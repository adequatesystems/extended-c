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


/* Initialize default configuration at runtime */
int Printlevel = PLEVEL_LOG;
int Outputlevel = PLEVEL_DEBUG;
FILE *Outputfp;

/* Mutually exclusive locks for protected prints and logs */
Mutex Printlock = MUTEX_INITIALIZER;
Mutex Outputlock = MUTEX_INITIALIZER;

/* Print/log counters */
unsigned Nprinterrs;
unsigned Nprintlogs;


/* strerror_r() is specified by POSIX.1-2001... */
#if ! defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L

static void strerror_r(int errnum, char *buf, size_t buflen)
{  /* ... provides fallback, when strerror_r() is unavailable */
   static Mutex lock = MUTEX_INITIALIZER;

   /* a NULL buf or a bufflen of zero (0) is inappropriate */
   if (buf == NULL || buflen == 0) return;
   /* secure the integrity of strerror()'s static string */
   mutex_lock(&lock);
   strncpy(buf, strerror(errnum), buflen);
   mutex_unlock(&lock);
}  /* end strerror_r() */

#endif


static void vfprintf_split_end(FILE *fp, const char *fmt, va_list args)
{
   char fmt_part[BUFSIZ], *nextp;
   char *fmtp = (char *) fmt;

   /* when printing to screen, it's possible to get garbled text
    * due to any movement of the cursor (e.g. psticky), so this
    * splits fmt into lines and clears right after every line */
   while((nextp = strpbrk(fmtp, "\r\n"))) {
      strncpy(fmt_part, fmtp, nextp - fmtp);
      fmt_part[nextp - fmtp] = '\0';
      /* print section without newline */
      vfprintf(fp, fmt_part, args);
      /* clear right of cursor and print character */
      if (nextp != fmt && nextp[-1] != '\r') {
         fprintf(fp, "\33[K%c", *(nextp++));
      } else fprintf(fp, "%c", *(nextp++));
      fmtp = nextp;
   }
   vfprintf(fp, fmtp, args);
   va_end(args);
}

/**
 * @brief Get the number of errors counted by extprint().
 * @returns Number of logs counted by extprint()
*/
unsigned get_num_errs(void)
{
   unsigned counter;

   mutex_lock(&Outputlock);
   counter = Nprinterrs;
   mutex_unlock(&Outputlock);

   return counter;
}

/**
 * @brief Get the number of logs counted by extprint().
 * @returns Number of logs counted by extprint()
*/
unsigned get_num_logs(void)
{
   unsigned counter;

   mutex_lock(&Outputlock);
   counter = Nprintlogs;
   mutex_unlock(&Outputlock);

   return counter;
}

/**
 * @brief Set the output file for printing logs to file.
 * @param fname file name of output file
 * @param mode file mode of output file (e.g. "w" or "a")
 * @returns 0 on success, else 1 on error (see @a errno for details)
 * @note if @a fname or @a mode are `NULL`, output file is closed
 * (if open) and function returns success
*/
int set_output_file(char *fname, char *mode)
{
   int ecode = 0;

   mutex_lock(&Outputlock);
   if (Outputfp) fclose(Outputfp);
   if (fname && mode) {
      Outputfp = fopen(fname, mode);
      if (Outputfp == NULL) ecode = 1;
   } else Outputfp = NULL;
   mutex_unlock(&Outputlock);

   return ecode;
}

/**
 * @brief Set the print level for printing to output.
 * @param level The print level allowed to print to output file
*/
void set_output_level(int level)
{
   mutex_lock(&Outputlock);
   Outputlevel = level;
   mutex_unlock(&Outputlock);
}

/**
 * @brief Set the print level for printing to screen.
 * @param level The print level allowed to print to screen
*/
void set_print_level(int level)
{
   mutex_lock(&Printlock);
   Printlevel = level;
   mutex_unlock(&Printlock);
}

/**
 * @brief Print a message.
 *
 * Print a (clean) message to stdout.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @note Does NOT print to output, ignores print level
*/
void print(const char *fmt, ...)
{
   va_list args;

   va_start(args, fmt);
   vfprintf_split_end(stdout, fmt, args);
}

/**
 * @brief Print to screen and log to file.
 * @param level print level of log
 * @param errnum error number ( @a errno ) associated with log
 * @param line the line number to place in the trace details
 * @param file the filename to place in the trace details
 * @param fmt A string format (or message) to log
 * @param ... Variable arguments supporting @a fmt
 * @returns 2 for PLEVEL_FATAL, 1 for PLEVEL_ERROR, else 0
 * @note This function is a multipurpose function designed for
 * use through the perrno(), perr(), plog(), and pdebug() MACROS.
*/
int print_ext(int errnum, int level, char *trace, const char *fmt, ...)
{
   char timestamp[32] = "";
   char prefix[128] = "";
   char suffix[64] = "";
   unsigned *counterp;
   int ecode = 0;
   struct tm dt;
   time_t t;
   FILE *fp;
   va_list args;

   /* build print configuration - based on level */
   switch(level) {
      case PLEVEL_FATAL:
         if (ecode == 0 && (ecode = 2) && fmt == NULL) return ecode;
         strncat(prefix, trace, sizeof(prefix) - strlen(prefix) - 1);
         strncat(prefix, "!!! FATAL ", sizeof(prefix) - strlen(prefix) - 1);
         /* fall through */
      case PLEVEL_ERROR:
         if (ecode == 0 && (ecode = 1) && fmt == NULL) return ecode;
         strncat(prefix, "Error. ", sizeof(prefix) - strlen(prefix) - 1);
         if (errnum >= 0) {
            strerror_r(errnum, &suffix[2], sizeof(suffix) - 3);
            suffix[0] = ':'; suffix[1] = ' ';
         }
         counterp = &Nprinterrs;
         fp = stderr;
         break;
      case PLEVEL_DEBUG:
         strncat(prefix, trace, sizeof(prefix) - strlen(prefix) - 1);
         strncat(prefix, "DEBUG: ", sizeof(prefix) - strlen(prefix) - 1);
         /* fall through */
      case PLEVEL_WARN:
         /* fall through */
      case PLEVEL_LOG:
         /* fall through */
      default:
         if (fmt == NULL) return ecode;
         counterp = &Nprintlogs;
         fp = stdout;
   }

   /* grab local time for log */
   time(&t);
   localtime_r(&t, &dt);

   /* print to screen, timestamp: "hh:mm:ss" */
   if (Printlevel >= level) {
      mutex_lock(&Printlock);

      strftime(timestamp, sizeof(timestamp) - 1, "%T - ", &dt);
      fprintf(fp, "%s%s", timestamp, prefix);
      va_start(args, fmt);
      vfprintf_split_end(fp, fmt, args);
      fprintf(fp, "%s\33[K\n", suffix);

      mutex_unlock(&Printlock);

      /* update sticky messages */
      psticky(NULL);
   }

   mutex_lock(&Outputlock);

   /* print to output, timestamp: "yyyy-mm-ddThh:mm:ss+0000" */
   if (Outputfp && Outputlevel >= level) {
      strftime(timestamp, sizeof(timestamp) - 1, "%FT%T%z - ", &dt);
      fprintf(Outputfp, "%s%s", timestamp, prefix);
      va_start(args, fmt);
      vfprintf(Outputfp, fmt, args);
      va_end(args);
      fprintf(Outputfp, "%s\n", suffix);
   }

   /* increment appropriate print counter */
   (*counterp)++;

   mutex_unlock(&Outputlock);

   return ecode;
}

/**
 * @brief Print a sticky message.
 *
 * Prints and "sticks" a message to the bottom of a terminal.
 * @param fmt A string format (or message) for printing
 * @param ... Variable arguments supporting the format string
 * @note To clear sticky message use @code psticky(""); @endcode
 * @note To reprint sticky message use @code psticky(NULL); @endcode
 * @note Message is only sticky when using the extended print
 * logging functions; perrno(), perr(), plog() and pdebug().
*/
void psticky(const char *fmt, ...)
{
   static Mutex lock = MUTEX_INITIALIZER;
   static char sticky[BUFSIZ] = "";
   va_list args;

   /* bail on print level NONE, or NULL update */
   if (Printlevel == PLEVEL_NONE) return;
   if (fmt == NULL && strlen(sticky) == 0) return;

   mutex_lock(&lock);

   if (fmt) {
      /* update sticky message */
      va_start(args, fmt);
      vsnprintf(sticky, BUFSIZ, fmt, args);
      va_end(args);
   }

   mutex_lock(&Printlock);

   /* (re)print sticky message */
   printf("\n\33[K");         /* go down a line, clear */
   printf("%s\33[K", sticky); /* print sticky message, clear */
   printf("\r\33[1A\33[K");   /* restore cursor position, clear */
   fflush(stdout);            /* flush stdout */

   mutex_unlock(&Printlock);

   mutex_unlock(&lock);
}


#endif  /* end EXTENDED_PRINT_C */
