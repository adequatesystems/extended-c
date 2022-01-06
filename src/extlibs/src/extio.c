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

#if OS_WINDOWS
   #include <direct.h>  /* for _mkdir() */

/* end OS_WINDOWS */
#elif OS_UNIX
   #include <sys/stat.h>  /* for mkdir() */

/* end OS_UNIX */
   #endif

/**
 * Get the number of logical cores available for use. Includes
 * hyper threaded cores.
 * @returns Number of logical cores available for use.
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
 * Copy a file from one location to another.
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
 * @brief Check if a file exists.
 * @param fname Name of file to check
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
 * @param fname Name of file to check
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
 * Opens @a fname in "append" mode, and immediately closes it.
 * @param fname Name of file to touch
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
 * @param dirpath Path of directory to be created
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

/**
 * @brief Read data directly into a buffer.
 *
 * Reads, at most, @a len bytes from @a fname into @a buff
 * @param buff Buffer pointer to place read bytes
 * @param len Maximum number of bytes to read
 * @param fname Name of file to read from
 * @returns Number of bytes read into buff, or (-1) on error
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

/**
 * @brief Write data from a buffer, directly to file.
 *
 * Writes, at most, @a len bytes from @a buff to @a fname
 * @param buff Buffer pointer of bytes to write
 * @param len Maximum number of bytes to write
 * @param fname Name of file to write to
 * @returns Number of bytes written, or (-1) on error.
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
