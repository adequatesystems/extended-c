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

   /* compatibility layer definitions for file mapping */
   #define MAP_FILE        0x00
   #define MAP_SHARED      0x01
   #define MAP_PRIVATE     0x02
   #define MAP_TYPE        0x0f
   #define MAP_FIXED       0x10
   #define MAP_ANONYMOUS   0x20
   #define MAP_ANON        MAP_ANONYMOUS
   #define MAP_FAILED      ((void *) -1)
   #define PROT_NONE       0x00
   #define PROT_READ       0x01
   #define PROT_WRITE      0x02
   #define PROT_EXEC       0x04

   /* uniform compatibility for cross-platform directory functions */
   #define cd(p)        _chdir(p)
   #define cwd(p, sz)   _getcwd(p, sz)

/* end Windows */
#else
   #include <unistd.h>
   #include <sys/stat.h>   /* for mkdir() */
   #include <sys/mman.h>  /* for mmap() et al */

   /* uniform compatibility for cross-platform directory functions */
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
int fbsearch(FILE *fp, const void *key, size_t len, void *buf, size_t size);
int fcopy(char *srcpath, char *dstpath);
int fexists(char *fpath);
int fexistsnz(char *fpath);
int fsave(FILE *stream, char *filename);
int fseek64(FILE *stream, long long offset, int origin);
long long ftell64(FILE *stream);
int ftouch(char *fpath);
int mkdir_p(char *dirpath);
size_t read_data(void *buff, size_t len, char *fpath);
size_t write_data(void *buff, size_t len, char *fpath);

#ifdef _WIN32

void *mmap(void *addr, size_t len, int prot, int flags, int fd, size_t off);
int munmap(void *addr, size_t length);

/* end Windows */
#endif

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
