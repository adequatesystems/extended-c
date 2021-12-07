/**
 * @private
 * @headerfile extio.h <extio.h>
 * @copyright © Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

#ifndef EXTENDED_INPUTOUTPUT_C
#define EXTENDED_INPUTOUTPUT_C  /* include guard */


#include "extio.h"
#include <errno.h>   /* for access to errno and error codes */
#include <string.h>  /* for string manipulation and comparison */

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h> /* for GetSystemInfo() (_WIN32 ONLY) */
#include <direct.h>  /* for _mkdir() (_WIN32 ONLY) */
#include <intrin.h>  /* for __cpuidex() (_WIN32 ONLY) */
#else
#include <sys/stat.h>  /* for mkdir() (UNIXLIKE ONLY) */
#include <unistd.h>  /* for sysconf() (UNIXLIKE ONLY) */
#endif


/**
 * @private
 * @brief Self contained 32-bit word datatype
*/
typedef unsigned int reg32;

/**
 * @private
 * @brief cpuid function call
*/
static inline void cpuid_call(reg32 regs[], reg32 func, reg32 subfunc)
{  /* ... architecture dependant calls for cpuid */
   #if defined(__arm__) || defined(__aarch64__) || defined(__powerpc__)
      /* unsupported architectures - clear registers */
      regs[0] = regs[1] = regs[2] = regs[3] = 0;
   #elif defined( _WIN32)  /* MSVC */
      __cpuidex((int *) regs, (int) func, (int) subfunc);
   #else  /* assume gcc */
      asm volatile ("cpuid"
         : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
         : "0" (func), "1" (0), "2" (subfunc)
      );
   #endif
}

/**
 * @private
 * @brief Get cpuid information.
 *
 * Calls cpuid with appropriate function and subfunction parameters,
 * placing the result in regs[] on success.
 * @returns 1 on success, else 0 if cpuid is unsupported or if the
 * function parameter exceeds the maximum supported parameter
*/
static inline int cpuid(reg32 regs[], reg32 func, reg32 subfunc)
{
   static const reg32 EXT_FUNC_MASK = 0x80000000U;
   static int support = (-1);
   static reg32 hfp_ex = 0;
   static reg32 hfp = 0;

   /* in the unlikely event that cpuid is not supported in this
    * environment, we run the risk of undefined behaviour... */
   if (support < 0) {
   #if defined(_WIN64) || defined(__x86_64__)
      support = 1;  /* assumed support on x86_64 systems*/
   #elif defined(_WIN32) /* assume x86_32 Windows */
      __asm {
         pushfd;           /* save EFLAGS */
         pushfd;           /* store EFLAGS */
         xor dword ptr [esp],200000h; /* invert the ID bit in stored EFLAGS */
         popfd;            /* load stored EFLAGS (id bit inverted) */
         pushfd;           /* store EFLAGS again (id maybe inverted) */
         pop eax;          /* eax= modified EFLAGS (id maybe inverted) */
         xor eax,[esp];    /* eax= whichever bits were changed */
         popfd;            /* restore original EFLAGS */
         and eax,200000h;  /* eax= zero if no support, else non-zero */
         mov support, eax; /* pass eax to support */
	   }
   #elif defined(__unix__)  /* assume x86_32 unix */
      reg32 _eax, _ebx;
      asm volatile (
         "pushfl\n\t"         /* save EFLAGS */
         "pushfl\n\t"         /* store EFLAGS */
         "popl\t%0\n\t"       /* _eax= stored EFLAGS */
         "movl\t%0, %1\n\t"   /* _ebx= stored EFLAGS */
         "xorl\t$0x200000, %0\n\t" /* invert the ID bit in stored EFLAGS */
         "pushl\t%0\n\t"      /* store modified EFLAGS (ID bit inverted) */
         "popfl\n\t"          /* load stored EFLAGS (ID bit inverted) */
         "pushfl\n\t"         /* store EFLAGS again (ID bit inverted?) */
         "popl\t%0\n\t"       /* _eax= modified EFLAGS (ID bit inverted?) */
         "popfl\n\t"          /* restore original EFLAGS */
         "xorl\t%1, %0\n\t"   /* _eax= whichever bits were changed */
         "andl\t$0x200000, %0\n\t" /* _eax= zero if no support */
         "movl\t%0, %2\n\t"   /* pass _eax to support */
         : "=&r" (_eax), "=&r" (_ebx), "=&r" (support)
      );
   #else  /* assume no support */
      support = 0;
   #endif
      /* ... also, in the lesser unlikely event that a function
       * parameter is not supported, it should be indicated... */
      if (support) {
         /* obtain highest "extended" function parameter */
         cpuid_call(regs, EXT_FUNC_MASK, 0);
         hfp_ex = regs[0];
         /* obtain highest "standard" function parameter */
         cpuid_call(regs, 0, 0);
         hfp = regs[0];
      }
   }

   /* only execute cpuid() calls where support is available, and... */
   if (support) {
      /* ... func is within highest function parameter */
      if (func & EXT_FUNC_MASK) {
         if (func > hfp_ex) return 0;
      } else if (func > hfp) return 0;
      cpuid_call(regs, func, subfunc);
      return 1;
   }

   return 0;
}  /* end cpuid() */

/**
 * @brief Get the Processor Vendor String (a.k.a. Manufacturer ID).
 * @returns Pointer to a static processor vendor string.
 * @note If CPUID does NOT support the processor vendor string,
 * vendor= "NotSupported"
*/
char *cpu_vendor(void)
{
   static char vendor[13] = { 0 };
   reg32 regs[4] = { 0 };

   if (vendor[0] == 0) {
      if (cpuid(regs, 0, 0)) {
         ((reg32 *) vendor)[0] = regs[1];  /* EBX */
         ((reg32 *) vendor)[1] = regs[3];  /* EDX */
         ((reg32 *) vendor)[2] = regs[2];  /* ECX */
      } else strncpy(vendor, "NotSupported", 13);
   }

   return vendor;
}  /* end cpuid_vendor() */

/**
 * @brief Get the Processor Brand String.
 * @returns Pointer to a static processor brand string.
 * @note If CPUID does NOT support the processor brand string,
 * brand= "Processor not supported..."
*/
char *cpu_brand(void)
{
   static reg32 brand[12] = { 0 };
   static char *brandp;
   char *cp;

   if (brand[0] == 0) {
      if (cpuid(brand, 0x80000004, 0)) {
         cpuid(&brand[0], 0x80000002, 0);
         cpuid(&brand[4], 0x80000003, 0);
         cpuid(&brand[8], 0x80000004, 0);
      } else strncpy((char *) brand, "Processor not supported...", 48);
      /* find first non-space character */
      cp = (char *) brand;
      while (*cp == ' ') cp++;
      brandp = cp;
   }

   return brandp;
}  /* end cpu_brand() */

/**
 * @brief Get the number of logical cores available for use.
 * @returns Number of logical cores available for use.
 * @note Includes Hyper Threads.
*/
int cpu_cores(void)
{
   static int cores = 0;

   if (cores == 0) {
   #ifdef _WIN32
      SYSTEM_INFO sysinfo;
      GetSystemInfo(&sysinfo);
      cores = (int) sysinfo.dwNumberOfProcessors;
   #else
      cores = (int) sysconf(_SC_NPROCESSORS_ONLN);
   #endif
   }

   return cores;
}  /* end cpu_cores() */

/**
 * @brief Get the L2 cache available to the processor cores.
 * @returns Amount of cache, in Kilobytes, or 0 on error.
*/
int cpu_cache(void)
{
   static int cache = -1;
   reg32 regs[4] = { 0 };

   if (cache < 0) {
      if (cpuid(regs, 0x80000006, 0)) {
         cache = (regs[2] >> 16) & 0xffff;  /* ECX */
      } else cache = 0;
   }

   return cache;
}  /* end cpu_cache() */

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
 * @warning The length of `dirpath` (including the nul-terminator)
 * MUST be less than `FILENAME_MAX`, otherwise `mkdir_p()` will
 * fail with `errno` set to `ENAMETOOLONG`.
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
   #ifdef _WIN32
      ecode = (_mkdir(path) && errno != EEXIST) ? 1 : 0;
   #else
      ecode = (mkdir(path, 0777) && errno != EEXIST) ? 1 : 0;
   #endif
   } while(ecode == 0 && len < strlen(dirpath));

   return ecode;
}

/* Read data from file, fname, into buff[len].
 * Returns read count or -1 on error.
*/
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
 * Returns write count or -1 on error.
*/
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


#endif  /* end EXTENDED_INPUTOUTPUT_C */
