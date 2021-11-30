/**
 * extprint.c - Extended print and logging support header
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 1 January 2018
 * Revised: 30 November 2021
 *
*/

#ifndef EXTENDED_PRINT_H
#define EXTENDED_PRINT_H  /* include guard */


#include <stdio.h>


/* Global Log Level definitions */
#define PLEVEL_NUL   0  /* Disables logs */
#define PLEVEL_ERR   1  /* Enables error type logs */
#define PLEVEL_LOG   2  /* Enables log/error type logs */
#define PLEVEL_BUG   3  /* Enables debug/log/error type logs */

#define PMESSAGE_MAX 1024

/* Function redirects for print progress bar function */
#define pprog_reprint()    pprog(NULL, NULL, 0L, 0L)
#define pprog_done(name)   pprog(name, NULL, -1L, -1L)


/* Print configuration. Log format:
 * [time(ptime=1)]; <pre><msg>[: details(func dependant)] */
typedef struct {
   int fd;     /* File descriptor for writing logs */
   FILE *fp;   /* File pointer for writing logs */
   void *ex;   /* Mutex lock pointer for exclusive writes */
   char level; /* Log level, PLEVEL_NUL for no logs */
   char time;  /* Prefix logs with timestamp, 0 for no timestamp */
   char *pre;  /* Prefix for logs (appends to timestamp) */
   unsigned nlogs;  /* Counts number of logs */
} PCONFIG;

typedef struct {
   PCONFIG perr;
   PCONFIG plog;
   PCONFIG pbug;
   PCONFIG err;
   PCONFIG out;
} PCONFIGCONTAINER;


#ifdef __cplusplus
extern "C" {
#endif

/* Global print configuration struct */
PCONFIGCONTAINER Pconfig;

/* Function prototypes for extio.c */
int perrno(int errnum, const char *fmt, ...);
int perr(const char *fmt, ...);
int plog(const char *fmt, ...);
int pbug(const char *fmt, ...);
void pprog(char *msg, char *unit, long cur, long end);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_PRINT_H */
