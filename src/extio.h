/**
 * @file extio.h
 * @brief Extended input/output support.
 * @details Provides extended support for system input and output.
 * @copyright Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_IO_H
#define EXTENDED_IO_H


#define _FILE_OFFSET_BITS  64 /* for 64-bit off_t stdio */
#include <stdio.h>

#ifdef _WIN32
   #include <win32lean.h>
   #include <direct.h>     /* for _mkdir() */

   #define cd(p)        _chdir(p)
   #define cwd(p, sz)   _getcwd(p, sz)

/* end Windows */
#else
   #include <unistd.h>
   #include <sys/stat.h>   /* for mkdir() */

   #define cd(p)        chdir(p)
   #define cwd(p, sz)   getcwd(p, sz)

/* end UNIX-like */
#endif

/* C/C++ compatible function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

int asnprintf(char *buf, size_t bufsz, const char *fmt, ...);
int cpu_cores(void);
void *fbsearch(char *fname, void *key, size_t len, void *buf, size_t size);
int fcopy(char *srcpath, char *dstpath);
int fexists(char *fpath);
int fexistsnz(char *fpath);
int fsave(FILE *stream, char *filename);
int fseek64(FILE *stream, long long offset, int origin);
long long ftell64(FILE *stream);
int ftouch(char *fpath);
int mkdir_p(char *dirpath);
int read_data(void *buff, int len, char *fpath);
int write_data(void *buff, int len, char *fpath);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
