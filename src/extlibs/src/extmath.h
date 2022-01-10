/**
 * @file extmath.h
 * @brief Extended math support.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

/* include guard */
#ifndef EXTENDED_MATH_H
#define EXTENDED_MATH_H


#include <math.h>


#ifdef __cplusplus
extern "C" {
#endif

/* x64 guard */
#ifdef HAS_64BIT

   /* x64 function prototypes for extmath.c */
   int iszero_x64(void *buff, int len); /**< @private */
   int add64_x64(void *ax, void *bx, void *cx); /**< @private */
   int sub64_x64(void *ax, void *bx, void *cx); /**< @private */
   void negate64_x64(void *ax); /**< @private */
   int cmp64_x64(void *ax, void *bx); /**< @private */
   int cmp256_x64(void *ax, void *bx); /**< @private */
   void shiftr64_x64(void *ax); /**< @private */
   int mult64_x64(void *ax, void *bx, void *cx); /**< @private */

/* end x64 guard */
#endif

/* x86 function prototypes for extmath.c */
int iszero_x86(void *buff, int len); /**< @private */
int add64_x86(void *ax, void *bx, void *cx); /**< @private */
int sub64_x86(void *ax, void *bx, void *cx); /**< @private */
void negate64_x86(void *ax); /**< @private */
int cmp64_x86(void *ax, void *bx); /**< @private */
int cmp256_x86(void *ax, void *bx); /**< @private */
void shiftr64_x86(void *ax); /**< @private */
int mult64_x86(void *ax, void *bx, void *cx); /**< @private */

/* Function prototypes for extmath.c */
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

/* end extern "C" {} for C++ */
#ifdef __cplusplus
}
#endif

/* end include guard */
#endif