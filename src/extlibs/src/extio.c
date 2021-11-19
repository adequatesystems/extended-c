/**
 * extio.c - Extended Input/Output support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 1 January 2018
 * Revised: 7 November 2021
 *
 * NOTES:
 * - For implementation of additional CPUID features;
 *    https://en.wikipedia.org/wiki/CPUID
 *
*/

#ifndef _EXTENDED_INPUTOUTPUT_C_
#define _EXTENDED_INPUTOUTPUT_C_  /* include guard */


#include "extio.h"
#include <errno.h>
#include <limits.h>  /* for determining CPUID* datatypes */
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

#ifdef DEBUG
int Pconsole = PCONSOLE_DEBUG;
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

#define MAXPROGRESS   16

/* determine suitable CPUID* datatype with 32-bit width */
#if ULONG_MAX == 0xFFFFFFFFUL
   /* long is preferred 32-bit word */
   typedef unsigned long int  CPUIDLeaf, CPUIDRegister;
#elif UINT_MAX == 0xFFFFFFFFU
   /* int is preferred 32-bit word */
   typedef unsigned int       CPUIDLeaf, CPUIDRegister;
#endif

/* CPUIDInfo struct for holding cpuid information */
typedef struct {
   CPUIDRegister EAX;
   CPUIDRegister EBX;
   CPUIDRegister ECX;
   CPUIDRegister EDX;
} CPUIDInfo;

/* Inline cpuid function */
static inline CPUIDInfo cpuidex(CPUIDLeaf leaf, CPUIDLeaf leafex) {
   CPUIDInfo info = { 0 };

#ifdef _WIN32
   __asm
   {
      mov    esi, info
      mov    eax, leaf
      mov    ecx, leafex
      cpuid
      mov    dword ptr [esi +  0], eax
      mov    dword ptr [esi +  4], ebx
      mov    dword ptr [esi +  8], ecx
      mov    dword ptr [esi + 12], edx
   }
#else
   asm volatile (
      "cpuid" :
         "=a" (info.EAX),
         "=b" (info.EBX),
         "=c" (info.ECX),
         "=d" (info.EDX)
      : "a" (leaf), "c" (leafex)
   );
#endif

   return info;
}  /* end cpuidex() */

/* Inline cpuid function for calls without extended information */
static inline CPUIDInfo cpuid(CPUIDLeaf leaf)
{
   return cpuidex(leaf, 0);
}  /* end cpuid() */

/* Obtain CPU Vendor information */
char *cpu_vendor(void)
{
   static char vendor[16] = { -1 };

   if (vendor[0] < 0) {
      CPUIDInfo info = cpuid(0);
      ((CPUIDRegister *) vendor)[0] = info.EBX;
      ((CPUIDRegister *) vendor)[1] = info.EDX;
      ((CPUIDRegister *) vendor)[2] = info.ECX;
   }

   return vendor;
}  /* end cpu_vendor() */

/* Obtain logical CPU cores */
int cpu_logical_cores(void)
{
   static int cores = -1;

   if (cores < 0) {
      CPUIDInfo info = cpuid(1);
      cores = (info.EBX >> 16) & 0xff;
   }

   return cores;
}  /* end cpu_logical_cores() */

/* Obtain Actual CPU cores */
int cpu_actual_cores(void)
{
   static int cores = -1;

   if (cores < 0) {
      /* use vendor dependant CPU information, else logical cores */
      if (strncmp(cpu_vendor(), "GenuineIntel", 16) == 0) {
         CPUIDInfo info = cpuid(4);
         cores = ((info.EAX >> 26) & 0x3f) + 1;
      } else if (!strncmp(cpu_vendor(), "AuthenticAMD", 16)) {
         CPUIDInfo info = cpuid(0x80000008);
         cores = ((info.ECX & 0xff)) + 1;
      } else return cpu_logical_cores();
   }

   return cores;
}  /* end cpu_actual_cores() */

/* Determine if hyper threads are enabled */
int cpu_hyper_threads(void)
{
   static int hyperthreads = -1;

   if (hyperthreads < 0) {
      CPUIDInfo info = cpuid(1);
      /* first check if capable of hyper-threading... */
      if (info.EDX & (1 << 28)) {
         /* ... then verify hyper-threading is enabled */
         hyperthreads = cpu_actual_cores() < cpu_logical_cores();
      } else hyperthreads = 0;
   }

   return hyperthreads;
}  /* end cpu_hyper_threads() */

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
   /* refresh any progress */
   pprog_update();
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
   /* refresh any progress */
   pprog_update();
}  /* end perr() */

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
   /* refresh any progress */
   pprog_update();
}  /* end plog() */

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
   /* refresh any progress */
   pprog_update();
}  /* end pdebug() */

void pprog(char *msg, char *unit, long cur, long end)
{
   static char metric[9][3] = { "", "K", "M", "G", "T", "P", "E", "Z", "Y" };
   static char spinner[] = "-\\|/";
   static struct _prog {
      float pc, ps;
      long tscur, cur, end, eta, time;
      char msg[48], unit[16];
      time_t ts, started;
   } prog[MAXPROGRESS], p;
   static size_t proglen = sizeof(*prog);
   static int count = 0;
   double diff;
   time_t now;
   int i, n;

   /* bail if NUL console printing or no progress */
   if (Pconsole == PCONSOLE_NUL || (!msg && !count)) return;

   /* determine update type */
   time(&now);
   if (msg) {
      for (i = 0; i <= count && i < MAXPROGRESS; i++) {
         if (prog[i].msg[0] == '\0') {
            /* create progress */
            prog[i].started = prog[i].ts = now;
            strncpy(prog[i].msg, msg, 48);
            if (unit) strncpy(prog[i].unit, unit, 16);
            if (cur) prog[i].cur = cur;
            if (end) prog[i].end = end;
            count++;
            break;
         } else if (strncmp(prog[i].msg, msg, 48) == 0) {
            prog[i].time = difftime(now, prog[i].started);
            if (cur < 0 && end < 0) {
               /* remove progress */
               if (count <= i) memset(&prog[i], 0, proglen);
               else memmove(&prog[i], &prog[i + 1], proglen * (count - i));
               i = count; while(i--) printf("\n\33[2K");
               printf("\33[%dA", count--);
            } else {
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
         printf("\n\33[2K%c %s... ", spinner[(int) p.time % 4], p.msg);
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
}

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

/**
 * Touch a file, result in either creation or opening and closing.
 * Returns 0 on success, else error code. */
int ftouch(char *fname)
{
   FILE *fp;

   fp = fopen(fname, "ab");
   if (fp == NULL) return errno;
   fclose(fp);

   return 0;
}

/**
 * Copy a file from one location, *srcname, to another, *dstname.
 * Returns 0 on success, else error code. */
int fcopy(char *srcname, char *dstname)
{
   char buf[BUFFER_SIZE];
   FILE *rfp, *wfp;
   size_t nBytes;
   int ecode = 0;

   rfp = fopen(srcname, "rb");
   wfp = fopen(dstname, "wb");
   if (rfp == NULL || wfp == NULL) return errno;
   while((nBytes = fread(buf, 1, BUFFER_SIZE, rfp))) {
      if (fwrite(buf, 1, nBytes, wfp) != nBytes) break;
   }
   if (ferror(rfp) || ferror(wfp)) ecode = errno;

   fclose(rfp);
   fclose(wfp);

   return ecode;
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
