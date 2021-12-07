/**
 * @file extio.h
 * @brief Extended input/output support.
 * @copyright © Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

#ifndef EXTENDED_INPUTOUTPUT_H
#define EXTENDED_INPUTOUTPUT_H  /* include guard */


#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extio.c */
char *cpu_vendor(void);
char *cpu_brand(void);
int cpu_cores(void);
int cpu_cache(void);
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


#endif  /* end EXTENDED_INPUTOUTPUT_H */
