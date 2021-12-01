/**
 * extprint.c - Extended print and logging support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 1 January 2018
 * Revised: 30 November 2021
 *
*/

#ifndef EXTENDED_PRINT_C
#define EXTENDED_PRINT_C  /* include guard */


#include "extprint.h"
#include "extthread.h"  /* for mutex support */
#include <stdarg.h>  /* for va_list support */
#include <string.h>  /* for strerror* functions */


#define MAXPROGRESS  16
#define ANYPCONFIGLEVEL (Perrcfg.level | Plogcfg.level \
   | Pbugcfg.level | Pstderrcfg.level | Pstdoutcfg.level)
#define PVFPRINTF(stdcfg, ll, msg, fmt, ap) \
   do { \
      va_start(ap, fmt); vsprintf(&msg[strlen(msg)], fmt, ap); va_end(ap); \
      msg[PMESSAGE_MAX - 1] = '\0'; \
      if (Perrcfg.level >= ll) pcfg(&(Perrcfg), vmsg); \
      if (Plogcfg.level >= ll) pcfg(&(Plogcfg), vmsg); \
      if (Pbugcfg.level >= ll) pcfg(&(Pbugcfg), vmsg); \
      if (stdcfg.level >= ll) pcfg(&(stdcfg), vmsg); \
   } while(0)


/* Define private mutex's for print functions and console */
Mutex Perrex = MUTEX_INITIALIZER;
Mutex Plogex = MUTEX_INITIALIZER;
Mutex Pbugex = MUTEX_INITIALIZER;
Mutex Pstdex = MUTEX_INITIALIZER;
/* Apply runtime print configuration defaults */
PCONFIG Perrcfg = { .ex = &Perrex, .level = PLEVEL_ERR, .pre = "Error: " };
PCONFIG Plogcfg = { .ex = &Plogex, .level = PLEVEL_LOG };
PCONFIG Pbugcfg = { .ex = &Pbugex, .level = PLEVEL_BUG, .pre = "DEBUG; " };
PCONFIG Pstderrcfg = { .fd = 2, .ex = &Pstdex, .level = PLEVEL_ERR };
PCONFIG Pstdoutcfg = { .fd = 1, .ex = &Pstdex, .level = PLEVEL_LOG };


/* strerror_r() is specified by POSIX.1-2001... */
#if ! defined(_POSIX_C_SOURCE) || _POSIX_C_SOURCE < 200112L

/**
 * Fallback function for strerror_r() where unavailable. */
static inline void strerror_r(int errnum, char *buf, size_t buflen)
{  /* ... provides fallback, when unavailable */
   static Mutex lock = MUTEX_INITIALIZER;  /* statically initialized Mutex */

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
 * Convert current time (seconds since Epoch) into string timestamp
 * based on ISO 8061 format, as local time. Result is placed in
 * char *dest, if provided, else uses static char *cp.
 * Returns char* to converted result, *dest. */
static inline char *timestamp(char *dest, size_t count)
{
   struct tm dt, *dtp = &dt;
   time_t t, *tp = &t;

   time(tp);
   localtime_r(tp, dtp);
   strftime(dest, count, "%FT%T%z ", dtp);

   return dest;
}  /* end timestamp() */

/**
 * Print a log using a specific configuration, pointed to by *cfg. */
static inline void pcfg (PCONFIG *cfg, char *msg)
{
   char rclear[] = "\33[0K";
   char tstr[28] = "";

   /* skip configurations without a writing destination or... */
   if (cfg->fp == NULL) {  /* ... set writing destination from fd, if set */
      if (cfg->fd == 1) cfg->fp = stdout;
      else if (cfg->fd == 2) cfg->fp = stderr;
      else return;
   }

   /* start eclusive write */
   mutex_lock((Mutex *) cfg->ex);

   /* increment log counter */
   cfg->nlogs++;
   /* print to file pointer, guarded */
   if (cfg->time) fprintf(cfg->fp, "%s", timestamp(tstr, 28));
   if (cfg->pre) fprintf(cfg->fp, "%s", cfg->pre);
   if (cfg->fp != stdout && cfg->fp != stderr) rclear[0] = '\0';
   fprintf(cfg->fp, "%s%s\n", msg, rclear);
   fflush(cfg->fp);

   /* end exclusive write */
   mutex_unlock((Mutex *) cfg->ex);

   /* reprint any current progress bars */
   if (cfg->fd) pprog_reprint();
}

/**
 * Print or update progress bar "name".
 * pprog_reprint() to update all progress bars.
 * pprog_done(name) to remove a progress bar. */
void pprog(char *name, char *unit, long cur, long end)
{
   static char metric[9][3] = { "", "K", "M", "G", "T", "P", "E", "Z", "Y" };
   static char spinner[] = "-\\|/";
   static struct _prog {
      float pc, ps;
      long tscur, cur, end, eta, time;
      char name[48], unit[16];
      time_t ts, started;
   } prog[MAXPROGRESS], p;
   static size_t proglen = sizeof(*prog);
   static int count = 0;
   double diff;
   time_t now;
   int i, n;

   /* bail if NUL console printing or no progress */
   if (Pstdoutcfg.level == PLEVEL_NUL || (!name && !count)) return;

   mutex_lock(&Pstdex);

   /* determine update type */
   time(&now);
   if (name) {
      for (i = 0; i <= count && i < MAXPROGRESS; i++) {
         if (cur < 0 && end < 0) {
            /* check for and remove progress */
            if (strncmp(prog[i].name, name, 48) == 0) {
               if (i < (count - 1)) {
                  memmove(&prog[i], &prog[count - 1], proglen);
               } else memset(&prog[i], 0, proglen);
               i = count; while(i--) printf("\n\33[2K");
               printf("\33[%dA", count--);
               break;
            }
         } else if (prog[i].name[0] == '\0') {
            /* create progress */
            prog[i].started = prog[i].ts = now;
            strncpy(prog[i].name, name, 48);
            if (unit) strncpy(prog[i].unit, unit, 16);
            if (cur) prog[i].cur = cur;
            if (end) prog[i].end = end;
            count++;
            break;
         } else if (strncmp(prog[i].name, name, 48) == 0) {
            prog[i].time = difftime(now, prog[i].started);
            /* update progress */
            if (cur) prog[i].cur = cur;
            if (end) prog[i].end = end;
            diff = difftime(now, prog[i].ts);
            if (diff) {
               prog[i].ps = (prog[i].cur - prog[i].tscur) / diff;
               prog[i].tscur = prog[i].cur;
               prog[i].ts = now;
            }
            if (prog[i].end > 0) {
               /* calculate ETA and completion */
               prog[i].pc = 100.0 * prog[i].cur / prog[i].end;
               if (prog[i].ps) {
                  prog[i].eta = (prog[i].end - prog[i].cur) / prog[i].ps;
               }
            }
            break;
         }
      }
   }

   /* display update */
   if (count) {
      printf("\33[2K");
      for (n = i = 0, p = prog[0]; i < count; n = 0, i++, p = prog[i]) {
         while(p.ps > 1000 && n++ < 9) p.ps /= 1000;
         printf("\n\33[2K%c %s... ", spinner[(int) p.time % 4], p.name);
         if (p.pc > 0) {  /* print w/ percentage */
            printf("%.02f%% (%.2f%s%s/s) | ETA: %lds | Elapsed: %lds",
               p.pc, p.ps, metric[n], p.unit, p.eta, p.time);
         } else {  /* print w/o percentage */
            printf("%ld (%.2f%s%s/s) | Elapsed: %lds",
               p.cur, p.ps, metric[n], p.unit, p.time);
         }
      }
      printf("\r\33[%dA", i);
      fflush(stdout);
   }

   mutex_unlock(&Pstdex);
}

/**
 * Print an error message (with description of errnum) using
 * Pconfigs: stderr, perr, plog and pbug.
 * Returns 2 if errnum is less than 0, else 1. */
int perrno(int errnum, const char *fmt, ...)
{
   int ecode = errnum < 0 ? 2 : 1;
   char vmsg[PMESSAGE_MAX] = "";
   va_list args;

   /* ignore NULL fmt's and NUL log level scenarios */
   if (fmt == NULL || ANYPCONFIGLEVEL < PLEVEL_ERR) return ecode;

   /* build variable message and print to all (incl. stderr) */
   if (errnum >= 0) {
      strerror_r(errnum, vmsg, sizeof(vmsg)); \
      strncat(vmsg, "; ", sizeof(vmsg) - strlen(vmsg)); \
   }
   PVFPRINTF(Pstderrcfg, PLEVEL_ERR, vmsg, fmt, args);

   return ecode;
}  /* end perrno() */

/**
 * Print an error message using Pconfigs: stderr, perr, plog and pbug.
 * Returns 0. */
int perr(const char *fmt, ...)
{
   char vmsg[PMESSAGE_MAX] = "";
   va_list args;

   /* ignore NULL fmt's and NUL log level scenarios */
   if (fmt == NULL || ANYPCONFIGLEVEL < PLEVEL_ERR) return 1;

   /* build variable message and print to all (incl. stderr) */
   PVFPRINTF(Pstderrcfg, PLEVEL_ERR, vmsg, fmt, args);

   return 1;
}  /* end perr() */

/**
 * Print a log message using Pconfigs: stdout, plog and pbug.
 * Returns 0. */
int plog(const char *fmt, ...)
{
   char vmsg[PMESSAGE_MAX] = "";
   va_list args;

   /* ignore NULL fmt's and NUL log level scenarios */
   if (fmt == NULL || ANYPCONFIGLEVEL < PLEVEL_LOG) return 0;

   /* build variable message and print to all (incl. stdout) */
   PVFPRINTF(Pstdoutcfg, PLEVEL_LOG, vmsg, fmt, args);

   return 0;
}  /* end plog() */

/**
 * Print a debug message using Pconfigs: stdout and pbug.
 * Returns 0. */
int pbug(const char *fmt, ...)
{
   char vmsg[PMESSAGE_MAX] = "";
   va_list args;

   /* ignore NULL fmt's and NUL log level scenarios */
   if (fmt == NULL || ANYPCONFIGLEVEL < PLEVEL_BUG) return 0;

   /* build variable message and print to all (incl. stdout) */
   PVFPRINTF(Pstdoutcfg, PLEVEL_BUG, vmsg, fmt, args);

   return 0;
}  /* end pbug() */


#endif  /* end EXTENDED_PRINT_C */
