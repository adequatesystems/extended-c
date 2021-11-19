/**
 * extint.c - Extended integer support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 2 January 2018
 * Revised: 25 October 2021
 *
 * NOTES:
 * - For prng's with greater statistical randomness, see PCG and Xoroshiro:
 *    https://en.wikipedia.org/wiki/Permuted_congruential_generator
 *    https://en.wikipedia.org/wiki/Xoroshiro128%2B
 *
*/

#ifndef _EXTENDED_INTEGER_C_
#define _EXTENDED_INTEGER_C_  /* include guard */


#include "extint.h"

/* Private number generator seeds for rand16fast() and rand16() */
volatile word32 Lseed = 1;
volatile word32 Lseed2 = 1;
volatile word32 Lseed3 = 362436069;
volatile word32 Lseed4 = 123456789;

/* Returns a 16-bit unsigned value from buff. */
word16 get16(void *buff)
{
   return *((word16 *) buff);
}

/* Places a 16-bit value in buff. */
void put16(void *buff, word16 val)
{
   *((word16 *) buff) = val;
}

/* Returns a 32-bit unsigned value from buff. */
word32 get32(void *buff)
{
   return *((word32 *) buff);
}

/* Places a 32-bit value in buff. */
void put32(void *buff, word32 val)
{
   *((word32 *) buff) = val;
}

/* Places a 64-bit unsigned value in buff. */
void put64(void *buff, void *val)
{
#ifdef WORD64_MAX
   put64_x64(buff, val);
#else
   put64_x86(buff, val);
#endif
}

/* Set the seed for the rand16fast() number generator to x. */
void srand16fast(word32 x)
{
   Lseed = x;
}

/* Get the current rolling seed for the rand16fast()
 * number generator, Lseed. Returns the value of Lseed. */
word32 get_rand16fast(void)
{
   return Lseed;
}

/* Set the seed values for the rand16() number generator,
 * Lseed2, Lseed3 and Lseed4, to x, y, and z, respectively. */
void srand16(word32 x, word32 y, word32 z)
{
   Lseed2 = x;
   Lseed3 = y;
   Lseed4 = z;
}

/* Place the current rolling seed values for the rand16()
 * number generator, Lseed2, Lseed3 and Lseed4, into
 * x, y, and z, respectively. */
void get_rand16(word32 *x, word32 *y, word32 *z)
{
   *x = Lseed2;
   *y = Lseed3;
   *z = Lseed4;
}

/* 16-bit (fast) prng using Lseed, based on Dr. Marsaglia's
 * Usenet post of a linear congruential generator.
 * Returns 0-65535. */
word32 rand16fast(void)
{
   Lseed = Lseed * WORD32_C(69069) + WORD32_C(262145);

   return Lseed >> 16;
}

/* 16-bit prng using Lseed2, Lseed3 and Lseed4, based on
 * Dr. Marsaglia's KISS method. Produces decent 16-bit
 * statistical randomness. Returns 0-65565. */
word32 rand16(void)
{
   /* linear congruential generator */
   Lseed2 = Lseed2 * WORD32_C(69069) + WORD32_C(262145);
   /* multiply with carry */
   if(Lseed3 == 0) Lseed3 = WORD32_C(362436069);
   Lseed3 = WORD32_C(36969) * (Lseed3 & WORD32_C(65535)) + (Lseed3 >> 16);
   /* linear-feedback shift register */
   if(Lseed4 == 0) Lseed4 = WORD32_C(123456789);
   Lseed4 ^= (Lseed4 << 17);
   Lseed4 ^= (Lseed4 >> 13);
   Lseed4 ^= (Lseed4 << 5);
   /* the KISS method (combination or methods) */
   return (Lseed2 ^ (Lseed3 << 16) ^ Lseed4) >> 16;
}

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


#endif  /* end _EXTENDED_INTEGER_C_ */
