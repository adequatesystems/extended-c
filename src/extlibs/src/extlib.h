/**
 * extlib.h - Extended general utilities support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 2 January 2018
 * Revised: 24 November 2021
 *
 * NOTES:
 * - "extends" the C Standard Library header file stdlib.h
 *
*/

#ifndef EXTENDED_UTILITIES_H
#define EXTENDED_UTILITIES_H  /* include guard */


#include <stdlib.h>
#include "extint.h"


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WORD64_MAX

/* x64 function prototypes */
void put64_x64(void *buff, void *val);

#endif

/* x86 function prototypes */
void put64_x86(void *buff, void *val);

/* Function prototypes */
word16 get16(void *buff);
void put16(void *buff, word16 val);
word32 get32(void *buff);
void put32(void *buff, word32 val);
void put64(void *buff, void *val);
void srand16fast(word32 x);
word32 get_rand16fast(void);
void srand16(word32 x, word32 y, word32 z);
void get_rand16(word32 *x, word32 *y, word32 *z);
word32 rand16fast(void);
word32 rand16(void);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_UTILITIES_H */
