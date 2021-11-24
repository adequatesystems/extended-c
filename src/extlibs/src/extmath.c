/**
 * extmath.c - Extended math support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 2 January 2018
 * Revised: 24 November 2021
 *
*/

#ifndef EXTENDED_MATH_C
#define EXTENDED_MATH_C  /* include guard */


#include "extmath.h"
#include "extint.h"
#include "extlib.h"


#ifdef WORD64_MAX  /* x64 guard */

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
 * 256-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx.
 * NOTE: uses 64-bit operations. */
int cmp256_x64(void *ax, void *bx)
{
   word64 *a = (word64 *) ax;
   word64 *b = (word64 *) bx;
   int i;

   for (i = 3; i >= 0; i--) {
      if(a[i] > b[i]) return 1;
      if(a[i] < b[i]) return -1;
   }

   return 0;
}  /* end cmp256_x64() */

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
 * 256-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx. */
int cmp256_x86(void *ax, void *bx)
{
   word32 *a = (word32 *) ax;
   word32 *b = (word32 *) bx;
   int i;

   for (i = 7; i >= 0; i--) {
      if(a[i] > b[i]) return 1;
      if(a[i] < b[i]) return -1;
   }

   return 0;
}  /* end cmp256_x86() */

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

/**
 * Check if buff is all zeros. Returns 1 on true, else 0. */
int iszero(void *buff, int len)
{
#ifdef WORD64_MAX
   return iszero_x64(buff, len);
#else
   return iszero_x86(buff, len);
#endif
}  /* end iszero() */

/**
 * 64-bit addition of *ax and *bx. Result in *cx. Returns carry. */
int add64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return add64_x64(ax, bx, cx);
#else
   return add64_x86(ax, bx, cx);
#endif
}  /* end add64() */

/**
 * 64-bit subtraction of *bx from *ax. Result in *cx. Returns carry. */
int sub64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return sub64_x64(ax, bx, cx);
#else
   return sub64_x86(ax, bx, cx);
#endif
}  /* end sub64() */

/**
 * Swap sign on 64-bit *ax. Equivalent to *ax multiplied by -1. */
void negate64(void *ax)
{
#ifdef WORD64_MAX
   negate64_x64(ax);
#else
   negate64_x86(ax);
#endif
}  /* end negate64() */


/**
 * 64-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx. */
int cmp64(void *ax, void *bx)
{
#ifdef WORD64_MAX
   return cmp64_x64(ax, bx);
#else
   return cmp64_x86(ax, bx);
#endif
}  /* end cmp64() */

/**
 * 256-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx. */
int cmp256(void *ax, void *bx)
{
#ifdef WORD64_MAX
   return cmp256_x64(ax, bx);
#else
   return cmp256_x86(ax, bx);
#endif
}  /* end cmp256() */

/**
 * 64-bit shift *ax one to the right. */
void shiftr64(void *ax)
{
#ifdef WORD64_MAX
   shiftr64_x64(ax);
#else
   shiftr64_x86(ax);
#endif
}  /* end shiftr64() */

/**
 * 64-bit multiplication of *ax and *bx. Place result in *cx.
 * Returns 1 if overflow, else 0. */
int mult64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return mult64_x64(ax, bx, cx);
#else
   return mult64_x86(ax, bx, cx);
#endif
}  /* end mult64() */

/* Multi-byte addition of ax[bytelen] and bx[bytelen].
 * Place result in cx[bytelen].  Returns carry. */
int multi_add(void *ax, void *bx, void *cx, int bytelen)
{
   word8 *a, *b, *c;
   int t, carry = 0;

   if(bytelen < 1) return 0;

   a = ax; b = bx; c = cx;
   for( ; bytelen; a++, b++, c++, bytelen--) {
      t = *a + *b + carry;
      carry = t >> 8;
      *c = t;
   }
   return carry;
}

/* Multi-byte subtraction of b[bytelen] from a[bytelen].
 * Place result in c[bytelen].  Returns carry. */
int multi_sub(void *ax, void *bx, void *cx, int bytelen)
{
   word8 *a, *b, *c;
   int t, carry = 0;

   if(bytelen < 1) return 0;

   a = ax; b = bx; c = cx;
   for( ; bytelen; a++, b++, c++, bytelen--) {
      t = *a - *b - carry;
      carry = (t >> 8) & 1;
      *c = t;
   }
   return carry;
}


#endif  /* end EXTENDED_MATH_C */
