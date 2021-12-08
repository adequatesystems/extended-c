/**
 * @private
 * @headerfile extprint.h <extprint.h>
 * @copyright © Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

#ifndef EXTENDED_PRINT_C
#define EXTENDED_PRINT_C  /* include guard */


#include "extprint.h"
#include "extthread.h"  /* for mutex support */
#include <stdarg.h>  /* for va_list support */
#include <string.h>  /* for string support */


/**
 * @private
 * Print configuration struct
*/
typedef struct {
   FILE *fp;      /* file pointer for writing logs to file */
   Mutex lock;    /* mutex pointer for protected writes to file pointers */
   char *prefix;  /* pointer to string literal prefix for logs */
   unsigned num;  /* counts the number of logs */
} PCONFIG;

/* stdout and stderr share a Mutex for protected writes to terminal */
Mutex Pstdlock = MUTEX_INITIALIZER;

/* Apply default print configuration on runtime initialization */
PCONFIG Perr = { .lock = MUTEX_INITIALIZER, .prefix = "Error. " };
PCONFIG Plog = { .lock = MUTEX_INITIALIZER };
PCONFIG Pdebug = { .lock = MUTEX_INITIALIZER, .prefix = "DEBUG; " };
char Plevel = PLEVEL_LOG;
char Ptime = 0;


/* strerror_r() is specified by POSIX.1-2001... */
#if ! defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L

static inline void strerror_r(int errnum, char *buf, size_t buflen)
{  /* ... provides fallback, when strerror_r() is unavailable */
   static Mutex lock = MUTEX_INITIALIZER;

   /* a NULL buf or a bufflen of zero (0) is inappropriate */
   if (buf == NULL || buflen == 0) return;
   /* secure the integrity of strerror()'s static string */
   mutex_lock(&lock);
   strncpy(buf, strerror(errnum), buflen);
   mutex_unlock(&lock);
   /* ensure buf is ALWAYS nul-terminated */
   buf[buflen - 1] = '\0';
}  /* end strerror_r() */

#endif


/**
 * @private
 * Convert current time (seconds since Epoch) into string timestamp
 * based on ISO 8061 format, as local time. Result is placed in
 * char *dest, if provided, else uses static char *cp.
 * Returns char* to converted result, *dest.
*/
static inline char *timestamp(char *dest, size_t count)
{
   struct tm dt;
   time_t t;

   time(&t);
   localtime_r(&t, &dt);
   strftime(dest, count, "%FT%T%z ", &dt);

   return dest;
}  /* end timestamp() */

/**
 * @brief Print a sticky message.
 *
 * Prints and "sticks" a message to the bottom of a terminal.
 * @param msg A string message to stick to terminal
 * @note To clear sticky message use @code psticky(""); @endcode
 * @note To reprint sticky message use @code psticky(NULL); @endcode
 * @note Message is only sticky when using the extended print
 * logging functions; perrno(), perr(), plog() and pdebug().
*/
void psticky(char *msg)
{
   static Mutex lock = MUTEX_INITIALIZER;
   static char sticky[BUFSIZ] = "";

   /* bail on print level NONE, or NULL update */
   if (Plevel == PLEVEL_NONE) return;
   if (msg == NULL && strlen(sticky) == 0) return;

   mutex_lock(&lock);

   /* update sticky message, if specified */
   if (msg) {
      strncpy(sticky, msg, BUFSIZ);
      sticky[BUFSIZ - 1] = '\0';
   }
   /* (re)print sticky message */
   if (sticky[0]) {
      mutex_lock(&Pstdlock);

      printf("\n\33[K");         /* go down a line */
      printf("%s", sticky);      /* clear and print sticky */
      printf("\r\33[1A\33[K");   /* restore cursor position */
      fflush(stdout);            /* flush stdout */

      mutex_unlock(&Pstdlock);
   }

   mutex_unlock(&lock);
}

/**
 * @private
 * Multipurpose variable argument list print function that prints
 * to print configuration, and file pointers when specified.
 * @note assumes va_start() has already been called
*/
static inline void pvfprintf
(PCONFIG*cfg, FILE *fp, char *fmt, va_list args, char *post)
{
   char fmt_part[BUFSIZ], *nextp;
   char *fmtp = (char *) fmt;
   char timestr[28] = "";
   va_list args2;

   if (fp) {
      /* copy argument list */
      va_copy(args2, args);

      mutex_lock(&Pstdlock);

      /* begin file pointer print... */
      /* if (Ptime) fprintf(fp, "%s", timestamp(timestr, 28)); */
      if (cfg && cfg->prefix) fprintf(fp, "%s", cfg->prefix);
      /* printf fmt in sections separated by newline characters */
      while((nextp = strpbrk(fmtp, "\r\n"))) {
         strncpy(fmt_part, fmtp, nextp - fmtp);
         fmt_part[nextp - fmtp] = '\0';
         /* print section without newline */
         vfprintf(fp, fmt_part, args2);
         /* clear right of cursor and print character */
         if (nextp != fmt && nextp[-1] != '\r') {
            fprintf(fp, "\33[K%c", *(nextp++));
         } else fprintf(fp, "%c", *(nextp++));
         fmtp = nextp;
      }
      /* print remaining fmt */
      vprintf(fmtp, args2);
      va_end(args2);
      /* print post string when supplied */
      if (post) fprintf(fp, ": %s\33[K\n", post);
      else fprintf(fp, "\33[K\n");
      fflush(fp);
      /* ... end file pointer print */

      mutex_unlock(&Pstdlock);

      /* update sticky messages - MUST BE AFTER unlock, to avoid deadlock */
      psticky(NULL);
   }

   if (cfg) {
      mutex_lock(&(cfg->lock));

      /* print to file pointer, if specified */
      if (cfg->fp) {
         /* begin print to file pointer... */
         if (Ptime) fprintf(cfg->fp, "%s", timestamp(timestr, 28));
         if (cfg->prefix) fprintf(cfg->fp, "%s", cfg->prefix);
         vfprintf(cfg->fp, fmt, args); va_end(args);
         if (post) fprintf(cfg->fp, ": %s\n", post);
         else fprintf(cfg->fp, "\n");
         fflush(cfg->fp);
         /* ... end print to file pointer */
      }

      /* increment counter in print config */
      cfg->num++;

      mutex_unlock(&(cfg->lock));
   }
}

/**
 * @brief Print a clean message.
 *
 * Prints clean messages to stdout by printing a "clear right"
 * VT100 escape code before all newline characters.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @note Bypasses print level checks.
*/
void print(const char *fmt, ...)
{
   va_list args;

   /* ignore NULL fmt's */
   if (fmt == NULL) return;

   va_start(args, fmt);
   pvfprintf(NULL, stdout, (char *) fmt, args, NULL);
   /* pvfprintf() does va_end() */
}

/**
 * @brief Print/log an error message, with description of @a errnum.
 * @param errnum @a errno or code to use for error description
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @returns 1 if @a errnum is less than 0, else 2.
*/
int perrno(int errnum, const char *fmt, ...)
{
   char errstr[64] = "";
   va_list args;
   FILE *fp;

   if (fmt) {
      /* set stderr if print level is appropriate */
      fp = (Plevel >= PLEVEL_ERR) ? stderr : NULL;
      va_start(args, fmt);  /* < MUST OCCUR BEFORE pvfprintf() */
      if (errnum >= 0) {
         /* obtain error description and print with print configuration */
         strerror_r(errnum, errstr, sizeof(errstr));
         pvfprintf(&Perr, fp, (char *) fmt, args, errstr);
      } else pvfprintf(&Perr, fp, (char *) fmt, args, NULL);
      /* pvfprintf() does va_end() */
   }

   return errnum < 0 ? 2 : 1;
}  /* end perrno() */

/**
 * @brief Print/log an error message.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @returns 1 always.
*/
int perr(const char *fmt, ...)
{
   va_list args;
   FILE *fp;

   if (fmt) {
      /* set stderr if print level is appropriate */
      fp = (Plevel >= PLEVEL_ERR) ? stderr : NULL;
      va_start(args, fmt);  /* < MUST OCCUR BEFORE pvfprintf() */
      pvfprintf(&Perr, fp, (char *) fmt, args, NULL);
      /* pvfprintf() does va_end() */
   }

   return 1;
}  /* end perr() */

/**
 * @brief Print/log a log message.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @returns 0 always.
*/
int plog(const char *fmt, ...)
{
   va_list args;
   FILE *fp;

   if (fmt) {
      /* set stdout if print level is appropriate */
      fp = (Plevel >= PLEVEL_LOG) ? stdout : NULL;
      va_start(args, fmt);  /* < MUST OCCUR BEFORE pvfprintf() */
      pvfprintf(&Plog, fp, (char *) fmt, args, NULL);
      /* pvfprintf() does va_end() */
   }

   return 0;
}  /* end plog() */

/**
 * @brief Print/log a debug message.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @returns 0 always.
*/
int pdebug(const char *fmt, ...)
{
   va_list args;
   FILE *fp;

   if (fmt) {
      /* set stdout if print level is appropriate */
      fp = (Plevel >= PLEVEL_DEBUG) ? stdout : NULL;
      va_start(args, fmt);  /* < MUST OCCUR BEFORE pvfprintf() */
      pvfprintf(&Pdebug, fp, (char *) fmt, args, NULL);
      /* pvfprintf() does va_end() */
   }

   return 0;
}  /* end pdebug() */

/**
 * @brief Get the number of logs counted by perrno() and perr().
 * @param fp File pointer open in "write" or "append" mode
 * @returns Number of logs counted by perrno() and perr()
*/
unsigned get_perr_counter(void)
{
   return Perr.num;
}

/**
 * @brief Get the number of logs counted by plog().
 * @param fp File pointer open in "write" or "append" mode
 * @returns Number of logs counted by plog()
*/
unsigned get_plog_counter(void)
{
   return Plog.num;
}

/**
 * @brief Get the number of logs counted by pdebug().
 * @param fp File pointer open in "write" or "append" mode
 * @returns Number of logs counted by pdebug()
*/
unsigned get_pdebug_counter(void)
{
   return Pdebug.num;
}

/**
 * @brief Set the print/log file pointer for perrno() and perr().
 * @param fp File pointer open in "write" or "append" mode
*/
void set_perr_fp(FILE *fp)
{
   Perr.fp = fp;
}

/**
 * @brief Set the print/log file pointer for plog().
 * @param fp File pointer open in "write" or "append" mode
*/
void set_plog_fp(FILE *fp)
{
   Plog.fp = fp;
}

/**
 * @brief Set the print/log file pointer for pdebug().
 * @param fp File pointer open in "write" or "append" mode
*/
void set_pdebug_fp(FILE *fp)
{
   Pdebug.fp = fp;
}

/**
 * @brief Set the print/log prefix for perrno() and perr().
 * @param prefix Pointer to desired prefix
*/
void set_perr_prefix(char *prefix)
{
   Perr.prefix = prefix;
}

/**
 * @brief Set the print/log prefix for plog().
 * @param prefix Pointer to desired prefix
*/
void set_plog_prefix(char *prefix)
{
   Plog.prefix = prefix;
}

/**
 * @brief Set the print/log prefix for pdebug().
 * @param prefix Pointer to desired prefix
*/
void set_pdebug_prefix(char *prefix)
{
   Pdebug.prefix = prefix;
}

/**
 * @brief Set the print level for printing to terminal.
 * @param level The print level allowed to print to terminal
*/
void set_print_level(char level)
{
   Plevel = level;
}

/**
 * @brief Set the timestamp flag for prints to file pointers.
 * @param time Value to apply to the timestamp flag (e.g. 1/0, on/off)
*/
void set_print_timestamp(char time)
{
   Ptime = time;
}


#endif  /* end EXTENDED_PRINT_C */
