/**
 * extio.c - Extended Input/Output support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 1 January 2018
 * Revised: 25 October 2021
 *
*/

#ifndef _EXTENDED_INPUTOUTPUT_C_
#define _EXTENDED_INPUTOUTPUT_C_  /* include guard */


#include "extio.h"

#ifndef DISABLE_PALL

   #include <stdarg.h>  /* for va_list in print functions */
   #include <string.h>  /* for strerror*() functions */
   #include <time.h>    /* for use in timestamp() */

   /* when POSIX strerror_r() is not available... */
   #ifndef strerror_r  /* ... provide fallback */
   #define strerror_r(ECODE, STR, LEN) strncpy(STR, strerror(ECODE), LEN)
   #endif

   /* when POSIX localtime_r() is not available... */
   #ifndef localtime_r  /* ... provide fallback */
   #define localtime_r(T, DT) DT=localtime(T)
   #endif

   /* macro definition for cleaner code duplication in print functions */
   #define PFPRINTF(FP, PRE, POST, FMT, ARGP) \
      if (FP != NULL) { \
         fprintf(FP, "%s", PRE); \
         va_start(ARGP, FMT); \
         vfprintf(FP, FMT, ARGP); \
         va_end(ARGP); \
         fprintf(FP, "%s", POST); \
      }

   /* Convert current time (seconds since Epoch) into string timestamp
    * based on ISO 8061 format, as local time. Result is placed in
    * char *dest, if provided, else uses static char *cp.
    * Returns char* to converted result. */
   static inline char *timestamp(char *dest, size_t count)
   {
      struct tm dt, *dtp = &dt;
      time_t t, *tp = &t;

      time(tp);
      localtime_r(tp, dtp);
      strftime(dest, count, "%FT%T%z; ", dtp);

      return dest;
   }  /* end timestamp() */

   #ifndef DISABLE_PERR
      /* Print an error message (with ecode description) to Pstderrfp,
       * Pstdoutfp and/or stderr. */
      void perrno(int ecode, char *fmt, ...)
      {
         char prestr[128] = "";
         char poststr[128] = ": ";
         va_list argp;

         /* ignore NULL fmt's */
         if(fmt == NULL) return;

         /* counter */
         Nstderrs++;
         /* build prefix */
         if (Ptimestamp) timestamp(prestr, 128);
         strncat(prestr, PPREFIX_ERR, 128 - strlen(prestr));
         /* build appended error description */
         strerror_r(ecode, &poststr[strlen(poststr)], 128);
         strncat(poststr, "\n", 128 - strlen(poststr));
         /* print log to stderr, stdout, and debug files, where enabled */
         PFPRINTF(Pstderrfp, prestr, poststr, fmt, argp);
         PFPRINTF(Pstdoutfp, prestr, poststr, fmt, argp);
         PFPRINTF(Pdebugfp, prestr, poststr, fmt, argp);
         /* ensure console level is appropriate and not already written to */
         if (Pconsole < PCONSOLE_ERR || Pstderrfp == stderr ||
            Pstdoutfp == stderr || Pdebugfp == stderr) return;
         PFPRINTF(stderr, prestr, poststr, fmt, argp);
         fflush(stderr);
      }  /* end perrno() */

      /* Print an error message to Pstderrfp, Pstdoutfp and/or stderr. */
      void perr(char *fmt, ...)
      {
         char prestr[128] = "";
         va_list argp;

         /* ignore NULL fmt's */
         if(fmt == NULL) return;

         /* counter */
         Nstderrs++;
         /* build prefix */
         if (Ptimestamp) timestamp(prestr, 128);
         strncat(prestr, PPREFIX_ERR, 128 - strlen(prestr));
         /* print log to stderr, stdout, and debug files, where enabled */
         PFPRINTF(Pstderrfp, prestr, "\n", fmt, argp);
         PFPRINTF(Pstdoutfp, prestr, "\n", fmt, argp);
         PFPRINTF(Pdebugfp, prestr, "\n", fmt, argp);
         /* ensure console level is appropriate and not already written to */
         if (Pconsole < PCONSOLE_ERR || Pstderrfp == stderr ||
            Pstdoutfp == stderr || Pdebugfp == stderr) return;
         PFPRINTF(stderr, prestr, "\n", fmt, argp);
         fflush(stderr);
      }  /* end perr() */
   #endif  /* end #ifndef DISABLE_PERROR */
   #ifndef DISABLE_PLOG
      /* Print a log message to Pstdoutfp and/or stdout. */
      void plog(char *fmt, ...)
      {
         char prestr[128] = "";
         va_list argp;

         /* ignore NULL fmt's */
         if(fmt == NULL) return;

         /* counter */
         Nstdouts++;
         /* build prefix */
         if (Ptimestamp) timestamp(prestr, 128);
         strncat(prestr, PPREFIX_LOG, 128 - strlen(prestr));
         /* print log to stdout, and debug files, where enabled */
         PFPRINTF(Pstdoutfp, prestr, "\n", fmt, argp);
         PFPRINTF(Pdebugfp, prestr, "\n", fmt, argp);
         /* ensure console level is appropriate and not already written to */
         if (Pconsole < PCONSOLE_LOG || Pstdoutfp == stdout ||
            Pdebugfp == stdout) return;
         PFPRINTF(stdout, prestr, "\n", fmt, argp);
         fflush(stdout);
      }  /* end plog() */
   #endif  /* end #ifndef DISABLE_PLOG */
   #ifndef DISABLE_PDEBUG
      /* Print a debug message to file pointer Logdebugfp and/or stdout. */
      void pdebug(char *fmt, ...)
      {
         char prestr[128] = "";
         va_list argp;

         /* ignore NULL fmt's */
         if(fmt == NULL) return;

         /* counter */
         Ndebugs++;
         /* build prefix */
         if (Ptimestamp) timestamp(prestr, 128);
         strncat(prestr, PPREFIX_DEBUG, 128 - strlen(prestr));
         /* print log to debug files, where enabled */
         PFPRINTF(Pdebugfp, prestr, "\n", fmt, argp);
         /* ensure console level is appropriate and not already written to */
         if (Pconsole < PCONSOLE_DEBUG || Pdebugfp == stdout) return;
         PFPRINTF(stdout, prestr, "\n", fmt, argp);
         fflush(stdout);
      }  /* end pdebug() */
   #endif  /* end #ifndef DISABLE_PDEBUG */
#endif  /* end #ifndef DISABLE_PALL */

/* Check if a file exists and contains data. Attribution: Thanks David!
 * Returns 1 if file exists non-zero, else 0. */
int existsnz(char *fname)
{
   FILE *fp;
   long len;

   fp = fopen(fname, "rb");
   if(!fp) return 0;
   fseek(fp, 0, SEEK_END);
   len = ftell(fp);
   fclose(fp);

   return len ? 1 : 0;
}

/* Check if a file exists. Returns 1 if file exists, else 0. */
int exists(char *fname)
{
   FILE *fp;

   fp = fopen(fname, "rb");
   if(!fp) return 0;
   fclose(fp);

   return 1;
}

/* Write data buff[len] to file, fname.
 * Returns write count or -1 on error. */
int write_data(void *buff, int len, char *fname)
{
   FILE *fp;
   size_t count;

   fp = fopen(fname, "wb");
   if(fp == NULL) return -1;
   count = fwrite(buff, 1, len, fp);
   fclose(fp);

   return (int) count;
}  /* end write_data() */

/* Read data from file, fname, into buff[len].
 * Returns read count or -1 on error. */
int read_data(void *buff, int len, char *fname)
{
   FILE *fp;
   size_t count;

   if(len == 0) return 0;
   fp = fopen(fname, "rb");
   if(fp == NULL) return -1;
   count = fread(buff, 1, len, fp);
   fclose(fp);

   return (int) count;
}  /* end read_data() */


#endif  /* end _EXTENDED_INPUTOUTPUT_C_ */
