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

#define BUFFER_SIZE     4096

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extio.c */
char *cpu_vendor(void);
int cpu_logical_cores(void);
int cpu_actual_cores(void);
int cpu_hyper_threads(void);
int existsnz(char *fname);
int exists(char *fname);
int mkdir_p(const char *dirpath);
int ftouch(char *fname);
int fcopy(char *srcname, char *dstname);
int write_data(void *buff, int len, char *fname);
int read_data(void *buff, int len, char *fname);

#ifdef __cplusplus
}
#endif


#endif  /* end _EXTENDED_INPUTOUTPUT_H_ */
