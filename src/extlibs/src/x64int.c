/**
 * x64int.c - 64-bit extended integer support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifndef X64_INTEGER_C
#define X64_INTEGER_C  /* include guard */


#include "extint.h"
#ifdef WORD64_MAX  /* x64 guard */

/**
 * Place a 64-bit unsigned *val, in *buff.
 * NOTE: uses 64-bit operations. */
void put64_x64(void *buff, void *val)
{
   *((word64 *) buff) = *((word64 *) val);
}

/**
 * Check if buff is all zeros. Returns 1 on true, else 0.
 * NOTE: uses 64-bit operations. */
int iszero_x64(void *buff, int len)
{
   word8 *bp = (word8 *) buff;

   for( ; len >= 8; bp += 8, len -= 8) if(*((word64 *) bp)) return 0;
   for( ; len; bp++, len--) if(*bp) return 0;
   return 1;
}  /* end iszero_x64() */

/**
 * 64-bit addition of *ax and *bx. Result in *cx. Returns carry.
 * NOTE: uses 64-bit operations. */
int add64_x64(void *ax, void *bx, void *cx)
{
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);
   word64 *c = (word64 *) cx;

   *c = a + b;
   return (*c < a);
}  /* end add64_x64() */

/**
 * 64-bit subtraction of *bx from *ax. Result in *cx. Returns carry.
 * NOTE: uses 64-bit operations. */
int sub64_x64(void *ax, void *bx, void *cx)
{  /* 64-bit function variant for sub64() */
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);
   word64 *c = (word64 *) cx;

   *c = a - b;
   return (*c > a);
}  /* end sub64_x64() */

/**
 * Swap sign on 64-bit *ax. Equivalent to *ax multiplied by -1.
 * NOTE: uses 64-bit operations. */
void negate64_x64(void *ax)
{
   *((word64*) ax) = ~(*((word64*) ax)) + 1;
}  /* end negate64_x64() */


/**
 * 64-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx.
 * NOTE: uses 64-bit operations. */
int cmp64_x64(void *ax, void *bx)
{
   word64 *a = (word64 *) ax;
   word64 *b = (word64 *) bx;

   if(*a > *b) return 1;
   if(*a < *b) return -1;
   return 0;
}  /* end cmp64_x64() */

/**
 * 64-bit shift *ax one to the right.
 * NOTE: uses 64-bit operations. */
void shiftr64_x64(void *ax)
{
   *((word64 *) ax) >>= 1;
}  /* end shiftr64_x64() */

/**
 * 64-bit multiplication of *ax and *bx. Place result in *cx.
 * Returns 1 if overflow, else 0.
 * NOTE: uses 64-bit operations. */
int mult64_x64(void *ax, void *bx, void *cx)
{
   word64 *c = (word64 *) cx;
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);

   *c = a * b;
   if (a == 0) return 0; /* avoid division by zero */
   if ((*c / a) == b) return 0; /* check overflow */
   return 1; /* result overflowed */
}  /* end mult64_x64() */


#endif  /* end WORD64_MAX */

#endif  /* end X64_INTEGER_C */
