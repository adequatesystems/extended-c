/**
 * @file extlib.h
 * @brief Extended general utilities support.
 * @details Provides extended support for general utilities.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_UTILITIES_H
#define EXTENDED_UTILITIES_H


#include "extint.h"
#include <stdlib.h>

/* C/C++ compatible function prototypes for extthread.c */
#ifdef __cplusplus
extern "C" {
#endif

/* 64-bit guard */
#ifdef HAS_64BIT
   void put64_x64(void *buff, void *val);

#endif

void put64_x86(void *buff, void *val);

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
word32 rand32(void);
void srand32(unsigned long long x);
void shuffle(void *list, size_t size, size_t count);
void shufflenz(void *list, size_t size, size_t count);

void *bsearch_len(const void *key, size_t len,
   const void *ptr, size_t count, size_t size);
int filesort(const char *filename, size_t size, size_t bufsz,
   int (*comp)(const void *, const void *));
#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
