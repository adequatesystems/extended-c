/**
 * extio.h - Extended Input/Output support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 14 September 2021
 * Revised: 4 November 2021
 *
 * NOTES:
 * - "extends" the C Standard Library header file stdio.h
 *
*/

#ifndef _EXTENDED_INPUTOUTPUT_H_
#define _EXTENDED_INPUTOUTPUT_H_  /* include guard */


#include <stdio.h>

/* Global definitions level of print enabled in console */
#define PCONSOLE_NUL   0  /* no print logs in console */
#define PCONSOLE_ERR   1  /* allows perr*() in console */
#define PCONSOLE_LOG   2  /* allows perr*(), and plog() in console */
#define PCONSOLE_DEBUG 3  /* allows perr*(), plog(), and pdebug() in console */

/* Log prefix literals, when enabled, are written to logs before
 * the log's message, but after the timestamp. */
#ifndef PPREFIX_ERR
#define PPREFIX_ERR     "Error. "
#endif
#ifndef PPREFIX_LOG
#define PPREFIX_LOG     ""
#endif
#ifndef PPREFIX_DEBUG
#define PPREFIX_DEBUG   "DEBUG: "
#endif

/* Function redirects for print progress */
#define pprog_done(name)   pprog(name, NULL, -1, -1)
#define pprog_update()     pprog(NULL, NULL, 0, 0)

#ifdef __cplusplus
extern "C" {
#endif

/* Global file pointers, when set, print appropriate information to file */
FILE *Pstderrfp;  /* file pointer for writing stderr logs */
FILE *Pstdoutfp;  /* file pointer for writing stdout logs */
FILE *Pdebugfp;   /* file pointer for writing debug logs */

/* Global flags, when set, indicate operational functionality */
char Ptimestamp;  /* prefix logs with timestamp */
char Pconsole;    /* console output loglevel */

/* Global log counters */
volatile unsigned Nstderrs;  /* counter for number of stderr logs */
volatile unsigned Nstdouts;  /* counter for number of stdout logs */
volatile unsigned Ndebugs;   /* counter for number of debug logs */

/* Function prototypes for extio.c */

/* Obtain CPU Vendor information */
char *cpu_vendor(void);
int cpu_logical_cores(void);
int cpu_actual_cores(void);
int cpu_hyper_threads(void);
void perrno(int ecode, char *fmt, ...);
void perr(char *fmt, ...);
void plog(char *fmt, ...);
void pdebug(char *fmt, ...);
void pprog(char *msg, char *unit, long cur, long end);
int existsnz(char *fname);
int exists(char *fname);
int write_data(void *buff, int len, char *fname);
int read_data(void *buff, int len, char *fname);

#ifdef __cplusplus
}
#endif


#endif  /* end _EXTENDED_INPUTOUTPUT_H_ */
