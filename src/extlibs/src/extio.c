/**
 * @file extio.c
 * @headerfile extio.h extio
 * @date 1 Jan 2018 (Revised 1 Dec 2021)
 * @brief Source file providing extended IO support.
 * @copyright © Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For more information, please refer to ../LICENSE
*/

#ifndef EXTENDED_IO_C
#define EXTENDED_IO_C  /* include guard */


#include "extio.h"
#include <errno.h>   /* for access to errno and error codes */
#include <limits.h>  /* for isolated 32-bit unsigned datatypes */
#include <string.h>  /* for string manipulation and comparison */

#ifdef _WIN32
#include <direct.h>  /* for _mkdir() */
#else
#include <sys/stat.h>  /* for mkdir() */
#define _mkdir(_path)   mkdir(_path, 0777)  /* compatibility */
#endif

/* Internal 32-bit word datatype for CPUID* handling */
#if ULONG_MAX == 0xFFFFFFFFUL
   /* long is preferred 32-bit word */
   typedef unsigned long int  CPUIDLeaf, CPUIDReg;
#elif UINT_MAX == 0xFFFFFFFFU
   /* int is preferred 32-bit word */
   typedef unsigned int       CPUIDLeaf, CPUIDReg;
#endif

/**
 * @private
 * Internal CPUIDInfo struct for holding cpuid information.
*/
typedef struct {
   CPUIDReg EAX;
   CPUIDReg EBX;
   CPUIDReg ECX;
   CPUIDReg EDX;
} CPUIDInfo;

/**
 * @private
 * Internal inline cpuid function for extended informatio.
*/
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

/**
 * @private
 * Internal inline cpuid function for standard information.
*/
static inline CPUIDInfo cpuid(CPUIDLeaf leaf)
{
   return cpuidex(leaf, 0);
}  /* end cpuid() */

/**
 * @brief Detect CPU vendor.
 *
 * Determine the CPU vendor used by the system
 * (e.g. AuthenticAMD, GenuineIntel, etc.).
 * @return A nul-terminated char* representing the CPU vendor.
*/
char *cpu_vendor(void)
{
   static char vendor[16] = { -1 };

   if (vendor[0] < 0) {
      CPUIDInfo info = cpuid(0);
      ((CPUIDReg *) vendor)[0] = info.EBX;
      ((CPUIDReg *) vendor)[1] = info.EDX;
      ((CPUIDReg *) vendor)[2] = info.ECX;
   }

   return vendor;
}  /* end cpu_vendor() */

/**
 * @brief Detect number of logical CPU cores.
 *
 * Determine the number of logical CPU cores
 * (incl. hyper threads) available to the system.
 * @return Number of logical CPU cores.
*/
int cpu_logical_cores(void)
{
   static int cores = -1;

   if (cores < 0) {
      CPUIDInfo info = cpuid(1);
      cores = (info.EBX >> 16) & 0xff;
   }

   return cores;
}  /* end cpu_logical_cores() */

/**
 * @brief Detect number of actual CPU cores.
 *
 * Determine the number of actual CPU cores
 * (excl. hyper threads) available to the system.
 * @return Number of actual CPU cores.
*/
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

/**
 * @brief Detect CPU hyper threading.
 *
 * Determines if hyper threads are enabled on this system.
 * @return 1 if hyper threads are enabled, else 0.
*/
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

/**
 * @brief Copy a file.
 *
 * Copy a file from one location, srcpath, to another, dstpath.
 * @param srcpath Path of the source file.
 * @param dstpath Path of the destination file.
 * @return 0 on success, or 1 on error (check errno for details).
*/
int fcopy(char *srcpath, char *dstpath)
{
   char buf[BUFSIZ];
   FILE *sfp, *dfp;
   size_t nBytes;
   int ecode = 1;

   /* open source file */
   sfp = fopen(srcpath, "rb");
   if (sfp != NULL) {
      /* open destination file */
      dfp = fopen(dstpath, "wb");
      if (dfp != NULL) {
         /* transfer bytes in BUFSIZ chunks (set by stdio) */
         do {  /* nBytes represents number of bytes read */
            nBytes = fread(buf, 1, BUFSIZ, sfp);
         } while(nBytes && nBytes == fwrite(buf, 1, nBytes, dfp));
         /* if no file errors, set operation success (0) */
         if (ferror(sfp) == 0 && ferror(dfp) == 0) ecode = 0;
         fclose(dfp);
      }
      fclose(sfp);
   }

   return ecode;
}  /* end fcopy() */

/**
 * @relatesalso fexistsnz
 * @brief Check if a file exists and contains data.
 *
 * Checks if a file exists by opening it in "read-only" mode.
 * @return 1 if file exists, else 0.
*/
int fexists(char *fname)
{
   FILE *fp;

   fp = fopen(fname, "rb");
   if(!fp) return 0;
   fclose(fp);

   return 1;
}  /* end fexists() */

/**
 * @brief Check if a file exists and contains data.
 * @return 1 if file exists and contains data, else 0.
 * @note Attribution: Thanks David!
 * @relatesalso fexists
*/
int fexistsnz(char *fname)
{
   FILE *fp;
   long len;

   fp = fopen(fname, "rb");
   if(!fp) return 0;
   fseek(fp, 0, SEEK_END);
   len = ftell(fp);
   fclose(fp);

   return len ? 1 : 0;
}  /* end fexistsnz() */

/**
 * @brief Touch a file.
 *
 * Opens the file, `fname`, in "append" mode, and immediately closes it.
 * @return 0 on success, else 1 on error (check errno for details).
 * @note Performs no other operations on the file.
*/
int ftouch(char *fname)
{
   FILE *fp;

   fp = fopen(fname, "ab");
   if (fp == NULL) return 1;
   fclose(fp);

   return 0;
}  /* end ftouch() */

/**
 * @brief Create a directory at dirpath (including any parent directories).
 *
 * Immitates the shell command @verbatim mkdir -p <dirpath> @endverbatim
 * @return 0 on success, or 1 on error (check errno for details).
 * @note Where `dirpath` already exists, `mkdir_p()` always succeeds.
 * @warning The length of `dirpath` (incl. terminator) MUST be less
 * than `FILENAME_MAX`, otherwise `mkdir_p()` will fail with `errno`
 * set to `ENAMETOOLONG`.
*/
int mkdir_p(char *dirpath)
{
   char path[FILENAME_MAX] = { 0 };
   char *dirpathp = dirpath;
   size_t len = 0;
   int ecode = 1;

   if (dirpath == NULL) errno = EINVAL;
   else if (strlen(dirpath) + 1 >= FILENAME_MAX) errno = ENAMETOOLONG;
   else do {
      /* split (excl. preceding path separator)... */
      dirpathp = strpbrk(++dirpathp, "\\/");
      if (dirpathp == NULL) len = strlen(dirpath);
      else len = dirpathp - dirpath;
      /* ... and rebuild directory path... */
      strncpy(path, dirpath, len);
      path[len] = '\0';  /* ensure nul-termination */
      /* ... creating parent directories, and ignoring EEXIST */
      ecode = (_mkdir(path) && errno != EEXIST) ? 1 : 0;
   } while(ecode == 0 && len < strlen(dirpath));

   return ecode;
}

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


#endif  /* end EXTENDED_IO_C */
