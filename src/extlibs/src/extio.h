/**
 * extio.h - Extended Input/Output support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 14 September 2021
 * Revised: 25 October 2021
 *
 * NOTES:
 * - "extends" the C Standard Library header file stdio.h
 * Regarding the removal of print functions at compile time
 * to remove the overhead of related function calls...
 * - define DISABLE_PALL to disable all print functions.
 * - define DISABLE_PERR to disable perrno() and perr().
 * - define DISABLE_PLOG to disable plog().
 * - define DISABLE_PDEBUG to disable pdebug().
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

/* Print function removal definitions. */
#ifdef DISABLE_PALL
#define DISABLE_PERR
#define DISABLE_PLOG
#define DISABLE_PDEBUG
#endif
#ifdef DISABLE_PERR
   #define perrno(ECODE, FMT, ...)  do { /* nothing */ } while(0)
   #define perr(FMT, ...)           do { /* nothing */ } while(0)
#endif
#ifdef DISABLE_PLOG
   #define plog(FMT, ...)           do { /* nothing */ } while(0)
#endif
#ifdef DISABLE_PDEBUG
   #define pdebug(FMT, ...)         do { /* nothing */ } while(0)
#endif

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
#ifndef DISABLE_PALL
   #ifndef DISABLE_PERR
      void perrno(int ecode, char *fmt, ...);
      void perr(char *fmt, ...);
   #endif
   #ifndef DISABLE_PLOG
      void plog(char *fmt, ...);
   #endif
   #ifndef DISABLE_PDEBUG
      void pdebug(char *fmt, ...);
   #endif
#endif  /* end #ifndef DISABLE_PALL */
int existsnz(char *fname);
int exists(char *fname);
int write_data(void *buff, int len, char *fname);
int read_data(void *buff, int len, char *fname);

#ifdef __cplusplus
}
#endif


#endif  /* end _EXTENDED_INPUTOUTPUT_H_ */
