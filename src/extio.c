/**
 * @private
 * @headerfile extio.h <extio.h>
 * @copyright Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_IO_C
#define EXTENDED_IO_C


#include "extio.h"

/* internal support */
#include "exterrno.h"

/* external support */
#include <stdarg.h>  /* for va_list functionality */
#include <stdlib.h>  /* for malloc() functionality */
#include <string.h>  /* for string manipulation and comparison */

#ifndef _WIN32
   #include <unistd.h>    /* for sysconf() */

#endif

/**
 * Append to a character string buffer. Calculates the remaining available
 * space in @a buf using @a bufsz and `strlen(buf)`, and appends at most
 * the remaining available space, less 1 (for the null terminator).
 * @param buf Pointer to character string buffer
 * @param bufsz Size of @a buf, in bytes
 * @param fmt Pointer to null-terminated format string
 * @param ... arguments specifying additional data to print per format
 * @returns (int) Number of characters (including the null terminator)
 * which would have been appended to @a buf if @a bufsz was ignored.
*/
int asnprintf(char *buf, size_t bufsz, const char *fmt, ...)
{
   va_list args;
   size_t cur;
   int count;

   cur = strlen(buf);
   va_start(args, fmt);
   count = vsnprintf(&buf[cur], bufsz > cur ? bufsz - cur : 0, fmt, args);
   va_end(args);

   return count;
}  /* end asnprintf() */

/**
 * Get the number of logical cores available for use.
 * Includes hyper threaded cores.
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
 * Perform a binary search for data in a file.
 * Assumes file is sorted (ascending) in @a size length binary elements.
 * @param fp FILE pointer to search in
 * @param key Pointer to data to search for
 * @param len Length of data in key to compare
 * @param buf Pointer to a buffer to place search data
 * @param size Length of each item in search data file
 * @returns (int) 1 if found, else 0; check errno for details
 * @exception errno=EINVAL A function parameter is invalid
 * @exception errno=0 Key not found
*/
int fbsearch(FILE *fp, const void *key, size_t len, void *buf, size_t size)
{
   long long mid, hi, low;
   int cond;

   set_errno(0);

   /* parameter check */
   if (fp == NULL || key == NULL || buf == NULL || len == 0 || size == 0) {
      set_errno(EINVAL);
      return 0;
   }

   /* set hi/lo positions for search */
   if (fseek64(fp, 0LL, SEEK_END) != 0) return 0;
   hi = ftell64(fp);
   if (hi == (-1)) return 0;
   hi = (hi / size) - 1;
   low = 0;
   /* perform binary search */
   while (low <= hi) {
      mid = (hi + low) / 2;
      if (fseek64(fp, mid * size, SEEK_SET) != 0) break;
      if (fread(buf, size, 1, fp) != 1) break;
      /* compare next middle value */
      cond = memcmp(key, buf, len);
      if (cond == 0) return 1;  /* found */
      if (cond < 0) hi = mid - 1;
      else low = mid + 1;
   }  /* end while */

   return 0;
}  /* end fbsearch() */

/**
 * Copy a file from one location to another.
 * @param srcpath Path of the source file.
 * @param dstpath Path of the destination file.
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int fcopy(char *srcpath, char *dstpath)
{
   char buf[BUFSIZ];
   FILE *sfp, *dfp;
   size_t nBytes;
   int ecode = -1;

   /* open source file */
   sfp = fopen(srcpath, "rb");
   if (sfp != NULL) {
      /* open destination file */
      dfp = fopen(dstpath, "wb");
      if (dfp != NULL) {
         /* transfer bytes in BUFSIZ chunks (set by stdio) */
         while ((nBytes = fread(buf, 1, BUFSIZ, sfp))) {
            if (nBytes > 0 && fwrite(buf, nBytes, 1, dfp) != 1) break;
            if (nBytes < BUFSIZ) break;  /* EOF or ERROR */
         }
         /* if no file errors, set operation success (0) */
         if (ferror(sfp) == 0 && ferror(dfp) == 0) ecode = 0;
         fclose(dfp);
      }
      fclose(sfp);
   }

   return ecode;
}  /* end fcopy() */

/**
 * Check if a file exists.
 * @param fpath Path to file to check
 * @returns 1 if file exists, else 0.
*/
int fexists(char *fpath)
{
   FILE *fp;

   fp = fopen(fpath, "rb");
   if(!fp) return 0;
   fclose(fp);

   return 1;
}  /* end fexists() */

/**
 * Check if a file exists and contains data.
 * Attribution: Thanks David!
 * @param fpath Path o file to check
 * @returns 1 if file exists and contains data, else 0.
*/
int fexistsnz(char *fpath)
{
   FILE *fp;
   long long len;

   fp = fopen(fpath, "rb");
   if(!fp) return 0;
   fseek64(fp, 0LL, SEEK_END);
   len = ftell64(fp);
   fclose(fp);

   return len ? 1 : 0;
}  /* end fexistsnz() */

/**
 * Save the contents of a file stream to a specified file location.
 * Useful if a stream was opened in-memory or as a temporary file,
 * which may not require a physical disk location to begin with.
 * Existing files will be overwritten.
 * @param stream Pointer to a file stream to save
 * @param filename Filename to save file data to
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int fsave(FILE *stream, char *filename)
{
   FILE *fp;
   size_t count;
   int ecode = 0;
   char buf[BUFSIZ];

   rewind(stream);

   fp = fopen(filename, "wb");
   if (fp == NULL) return EOF;

   while ((count = fread(buf, 1, BUFSIZ, stream))) {
      if (count > 0 && fwrite(buf, count, 1, fp) != 1) break;
      if (count < BUFSIZ) break;
   }
   /* check errors */
   if (ferror(stream) || ferror(fp)) ecode = -1;

   /* cleanup */
   fclose(fp);

   return ecode;
}  /* end fsave() */

/**
 * Set the file position indicator of a file stream to a 64-bit offset.
 * @param stream Pointer to a FILE stream
 * @param offset Number of characters to shift the position from origin
 * @param origin Poisition to which offset is added
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int fseek64(FILE *stream, long long offset, int origin)
{
#ifdef _WIN32
   /* Windows' long 32-bit datatype is always insufficient */
   return _fseeki64(stream, offset, origin);

#else
   /* _FILE_OFFSET_BITS == 64, is defined in extio.h */
   return fseeko(stream, (off_t) offset, origin);

#endif
}  /* end fseek64() */

/**
 * Get the file position indicator for a file stream.
 * @param stream Pointer to a FILE stream to examine
 * @returns Value of file position offset on success, or (-1) on error.
 * Check errno for details.
*/
long long ftell64(FILE *stream)
{
#ifdef _WIN32
   /* Windows' long 32-bit datatype is always insufficient */
   return _ftelli64(stream);

#else
   /* _FILE_OFFSET_BITS == 64, is defined in extio.h */
   return ftello(stream);

#endif
}  /* end ftell64() */

/**
 * Touch a file. Opens @a fpath in "ab" mode, and closes it.
 * Performs no other operations on the file.
 * @param fpath Path to file to touch
 * @return 0 on success, or non-zero on error. Check errno for details.
*/
int ftouch(char *fpath)
{
   FILE *fp;

   fp = fopen(fpath, "ab");
   if (fp == NULL) return -1;
   fclose(fp);

   return 0;
}  /* end ftouch() */

/**
 * Create a directory at dirpath (including any parent directories).
 * Immitates the shell command @code mkdir -p <dirpath> @endcode
 * @param dirpath Path of directory to be created
 * @return 0 on success, or non-zero on error. Check errno for details.
 * @note Where `dirpath` already exists, `mkdir_p()` always succeeds.
*/
int mkdir_p(char *dirpath)
{
   char path[BUFSIZ] = { 0 };
   char *dirpathp = dirpath;
   size_t len = 0;
   int ecode = -1;

   if (dirpath == NULL) set_errno(EINVAL);
   else if (strlen(dirpath) + 1 >= BUFSIZ) set_errno(ENAMETOOLONG);
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
      ecode = (_mkdir(path) && errno != EEXIST) ? -1 : 0;
#else
      ecode = (mkdir(path, 0777) && errno != EEXIST) ? -1 : 0;
#endif
   } while(ecode == 0 && len < strlen(dirpath));

   return ecode;
}

/**
 * Read data directly into a buffer.
 * Reads, at most, @a len bytes from @a fpath into @a buff
 * @param buff Buffer pointer to place read bytes
 * @param len Maximum number of bytes to read
 * @param fpath Path to file to read from
 * @returns Number of bytes read into buff, or (-1) on error
*/
size_t read_data(void *buff, size_t len, char *fpath)
{
   FILE *fp;
   size_t count;

   if(len == 0) return 0;
   fp = fopen(fpath, "rb");
   if(fp == NULL) return -1;
   count = fread(buff, 1, len, fp);
   fclose(fp);

   return count;
}  /* end read_data() */

/**
 * Write data from a buffer, directly to file.
 * Writes, at most, @a len bytes from @a buff to @a fpath
 * @param buff Buffer pointer of bytes to write
 * @param len Maximum number of bytes to write
 * @param fpath Path to file to write to
 * @returns Number of bytes written, or (-1) on error.
*/
size_t write_data(void *buff, size_t len, char *fpath)
{
   FILE *fp;
   size_t count;

   fp = fopen(fpath, "wb");
   if(fp == NULL) return -1;
   count = fwrite(buff, 1, len, fp);
   fclose(fp);

   return count;
}  /* end write_data() */

/* Windows compatibility functions */
#ifdef _WIN32

/**
 * Open a file mapping to the specified file descriptor.
 * @note This is a Windows API compatibility layer function that
 * immitates, as close as reasonably possible, the functionality
 * available to UNIX systems under the <sys/mman.h> header.
*/
void *mmap(void *addr, size_t len, int prot, int flags, int fd, size_t off)
{
   HANDLE fileh, maph;
   void *mapp;
   int page;
   int view;

   /* sanity checks:
    * - len cannot be Zero
    * - MAP_FIXED is unsupported on Windows
    * - PROT_EXEC must be accompanied by additional protections */
   if (!len || (flags & MAP_FIXED) || (prot == PROT_EXEC)) {
      set_errno(EINVAL);
      return MAP_FAILED;
   }

   /* determine page protection access */
   if (flags & MAP_PRIVATE) {
      if (prot & PROT_EXEC) {
         page = (prot & PROT_WRITE)
            ? PAGE_EXECUTE_WRITECOPY : PAGE_EXECUTE_READ;
      } else page = (prot & PROT_WRITE) ? PAGE_WRITECOPY : PAGE_READONLY;
   } else if (prot & PROT_EXEC) {
      page = (prot & PROT_WRITE)
         ? PAGE_EXECUTE_READWRITE : PAGE_EXECUTE_READ;
   } else page = (prot & PROT_WRITE) ? PAGE_READWRITE : PAGE_READONLY;

   /* determine view protection access */
   if (prot & (PROT_READ | PROT_WRITE)) view |= FILE_MAP_ALL_ACCESS;
   else if (prot & PROT_READ) view |= FILE_MAP_READ;
   else if (prot & PROT_WRITE) view |= FILE_MAP_WRITE;
   /* ... add execute / copy-on-write flags */
   if (prot & PROT_EXEC) view |= FILE_MAP_EXECUTE;
   if (flags & MAP_PRIVATE) view |= FILE_MAP_COPY;

   /* derive file handle from file descriptor, flags permitting */
   if (!(flags & MAP_ANONYMOUS)) {
      fileh = (HANDLE) _get_osfhandle(fd);
      if (fileh == INVALID_HANDLE_VALUE) {
         set_errno(EBADF);
         return MAP_FAILED;
      }
   } else fileh = INVALID_HANDLE_VALUE;

   /* obtain file mapping handle */
   maph = CreateFileMapping(fileh, NULL, page, 0, (DWORD) off + len, NULL);
   if (maph == INVALID_HANDLE_VALUE) goto FAIL_MAP;

   /* obtain pointer to file mapping */
   mapp = MapViewOfFile(maph, view, 0, (DWORD) off, len);
   CloseHandle(maph);
   if (mapp == NULL) goto FAIL_MAP;

   /* success */
   return mapp;

/* error handling */
FAIL_MAP:
   set_alterrno(GetLastError());
   return MAP_FAILED;
}  /* end mmap() */

/**
 * Closes a file mapping.
 * @note This is a Windows API compatibility layer function that
 * immitates, as close as reasonably possible, the functionality
 * available to UNIX systems under the <sys/mman.h> header.
*/
int munmap(void *addr, size_t length)
{
   if (!UnmapViewOfFile(addr)) {
      set_alterrno(GetLastError());
      return -1;
   }

   return 0;
}  /* end munmap() */

/* end Windows */
#endif

/* end include guard */
#endif
