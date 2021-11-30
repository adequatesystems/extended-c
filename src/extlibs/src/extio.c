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
#include <string.h>  /* for string manipulation functions */
#include <time.h>    /* for use in timestamp() */

#ifdef _WIN32
#include <direct.h>  /* for _mkdir() */
#else
#include <sys/stat.h>  /* for mkdir() */
#define _mkdir(_path)   mkdir(_path, 0777)
#endif

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
 * Create directory at dirpath, including parents where specified.
 * Returns 0 on success (or if directory exists), else errno. */
int mkdir_p(const char *dirpath)
{
   char path[FILENAME_MAX] = { 0 };
   char *dirpathp = (char *) dirpath;
   size_t len = 0;

   if (dirpath == NULL) return EFAULT;
   if (strlen(dirpath) >= FILENAME_MAX - 1) return E2BIG;

   do {
      dirpathp = strpbrk(++dirpathp, "\\/");
      if (dirpathp == NULL) len = strlen(dirpath);
      else len = dirpathp - dirpath;
      strncpy(path, dirpath, len);
      path[len] = '\0';  /* ensure nul-termination */
      if (_mkdir(path) && errno != EEXIST) return errno;
   } while(len < strlen(dirpath));

   return 0;
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
   if (rfp == NULL) ecode = errno ? errno : EIO;
   else {
      wfp = fopen(dstname, "wb");
      if (wfp == NULL) ecode = errno ? errno : EIO;
      else {
         do {  /* perform copy operation */
            nBytes = fread(buf, 1, BUFFER_SIZE, rfp);
         } while(nBytes && fwrite(buf, 1, nBytes, wfp) != nBytes);
         if (ferror(rfp) || ferror(wfp)) ecode = errno ? errno : EIO;
         fclose(wfp);
      }
      fclose(rfp);
   }

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
