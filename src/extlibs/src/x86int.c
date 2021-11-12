/**
 * x86int.c - 32-bit extended integer support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifndef X86_INTEGER_C
#define X86_INTEGER_C  /* include guard */


#include "extint.h"

/**
 * Place a 64-bit unsigned *val, in *buff. */
void put64_x86(void *buff, void *val)
{
   ((word32 *) buff)[0] = ((word32 *) val)[0];
   ((word32 *) buff)[1] = ((word32 *) val)[1];
}

/**
 * Check if buff is all zeros. Returns 1 on true, else 0. */
int iszero_x86(void *buff, int len)
{
   word8 *bp = (word8 *) buff;

   for( ; len >= 4; bp += 4, len -= 4) if(*((word32 *) bp)) return 0;
   for( ; len; bp++, len--) if(*bp) return 0;
   return 1;
}  /* end iszero_x86() */

/**
 * 64-bit addition of *ax and *bx. Result in *cx. Returns carry. */
int add64_x86(void *ax, void *bx, void *cx)
{
   word32 a[2], b[2];
   word32 *c = (word32 *) cx;

   put64(a, ax);
   put64(b, bx);
   c[0] = a[0] + b[0];
   c[1] = a[1] + b[1] + (word32) (c[0] < a[0]);
   return (c[1] < a[1] || (c[1] == a[1] && c[0] < a[0]));
}  /* end add64_x86() */

/**
 * 64-bit subtraction of *bx from *ax. Result in *cx. Returns carry. */
int sub64_x86(void *ax, void *bx, void *cx)
{
   word32 a[2], b[2];
   word32 *c = (word32 *) cx;

   put64(a, ax);
   put64(b, bx);
   c[0] = a[0] - b[0];
   c[1] = a[1] - b[1] - (c[0] > a[0]);
   return (c[1] > a[1] || (c[1] == a[1] && c[0] > a[0]));
}  /* end sub64_x86() */

/**
 * Swap sign on 64-bit *ax. Equivalent to *ax multiplied by -1. */
void negate64_x86(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] = ~a[0];
   a[1] = ~a[1];
   if(++a[0] == 0) a[1]++;
}  /* end negate64_x86() */


/**
 * 64-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx. */
int cmp64_x86(void *ax, void *bx)
{
   word32 *a = (word32 *) ax;
   word32 *b = (word32 *) bx;

   if(a[1] > b[1]) return 1;
   if(a[1] < b[1]) return -1;
   if(a[0] > b[0]) return 1;
   if(a[0] < b[0]) return -1;
   return 0;
}  /* end cmp64_x86() */

/**
 * 64-bit shift *ax one to the right. */
void shiftr64_x86(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] >>= 1;
   if(a[1] & 1) a[0] |= 0x80000000;
   a[1] >>= 1;
}  /* end shiftr64_x86() */

/**
 * 64-bit multiplication of *ax and *bx. Place result in *cx.
 * Returns 1 if overflow, else 0. */
int mult64_x86(void *ax, void *bx, void *cx)
{
   word32 *c = (word32 *) cx;
   word32 a[2], b[2];
   int overflow = 0;

   put64(a, ax);
   put64(b, bx);
   c[0] = c[1] = 0;
   while(b[0] | b[1]) {
      if(b[0] & 1)
         overflow |= add64(c, a, c);
      add64(a, a, a);  /* shift a left */
      shiftr64(b);
   }

   return overflow;
}  /* end mult64_x86() */


#endif  /* end X86_INTEGER_C */
