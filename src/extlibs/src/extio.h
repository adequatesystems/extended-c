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

#ifndef EXTENDED_IO_H
#define EXTENDED_IO_H  /* include guard */


#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extio.c */
char *cpu_vendor(void);
int cpu_logical_cores(void);
int cpu_actual_cores(void);
int cpu_hyper_threads(void);
int fexists(char *fname);
int fexistsnz(char *fname);
int ftouch(char *fname);
int fcopy(char *srcpath, char *dstpath);
int mkdir_p(char *dirpath);
int write_data(void *buff, int len, char *fname);
int read_data(void *buff, int len, char *fname);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_IO_H */
