/**
 * extmath.h - Extended math support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 2 January 2018
 * Revised: 24 November 2021
 *
 * NOTES:
 * - "extends" the C Standard Library header file math.h
 *
*/

#ifndef EXTENDED_MATH_H
#define EXTENDED_MATH_H  /* include guard */


#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef WORD64_MAX

/* x64 function prototypes */
int iszero_x64(void *buff, int len);
int add64_x64(void *ax, void *bx, void *cx);
int sub64_x64(void *ax, void *bx, void *cx);
void negate64_x64(void *ax);
int cmp64_x64(void *ax, void *bx);
int cmp256_x64(void *ax, void *bx);
void shiftr64_x64(void *ax);
int mult64_x64(void *ax, void *bx, void *cx);

#endif

/* x86 function prototypes */
int iszero_x86(void *buff, int len);
int add64_x86(void *ax, void *bx, void *cx);
int sub64_x86(void *ax, void *bx, void *cx);
void negate64_x86(void *ax);
int cmp64_x86(void *ax, void *bx);
int cmp256_x86(void *ax, void *bx);
void shiftr64_x86(void *ax);
int mult64_x86(void *ax, void *bx, void *cx);

/* Function prototypes */
int iszero(void *buff, int len);
int add64(void *ax, void *bx, void *cx);
int sub64(void *ax, void *bx, void *cx);
void negate64(void *ax);
int cmp64(void *ax, void *bx);
int cmp256(void *ax, void *bx);
void shiftr64(void *ax);
int mult64(void *ax, void *bx, void *cx);
int multi_add(void *ax, void *bx, void *cx, int bytelen);
int multi_sub(void *ax, void *bx, void *cx, int bytelen);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_MATH_H */
