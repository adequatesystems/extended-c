/**
 * @file extmath.h
 * @brief Extended math support.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_MATH_H
#define EXTENDED_MATH_H


#include <math.h>

/* C/C++ compatible function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

/* x64 guard */
#ifdef HAS_64BIT
   int iszero_x64(const void *buff, int len);
   int add64_x64(const void *ax, const void *bx, void *cx);
   int sub64_x64(const void *ax, const void *bx, void *cx);
   void negate64_x64(void *ax);
   int cmp64_x64(const void *ax, const void *bx);
   int cmp256_x64(const void *ax, const void *bx);
   void shiftr64_x64(void *ax);
   int mult64_x64(const void *ax, const void *bx, void *cx);

/* end x64 guard */
#endif

int iszero_x86(const void *buff, int len);
int add64_x86(const void *ax, const void *bx, void *cx);
int sub64_x86(const void *ax, const void *bx, void *cx);
void negate64_x86(void *ax);
int cmp64_x86(const void *ax, const void *bx);
int cmp256_x86(const void *ax, const void *bx);
void shiftr64_x86(void *ax);
int mult64_x86(const void *ax, const void *bx, void *cx);

int iszero(const void *buff, int len);
int add64(const void *ax, const void *bx, void *cx);
int sub64(const void *ax, const void *bx, void *cx);
void negate64(void *ax);
int cmp64(const void *ax, const void *bx);
int cmp256(const void *ax, const void *bx);
void shiftr64(void *ax);
int mult64(const void *ax, const void *bx, void *cx);
int multi_add(const void *ax, const void *bx, void *cx, int bytelen);
int multi_sub(const void *ax, const void *bx, void *cx, int bytelen);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif