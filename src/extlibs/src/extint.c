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
void put64_32(void *buff, void *val)
{
   ((word32 *) buff)[0] = ((word32 *) val)[0];
   ((word32 *) buff)[1] = ((word32 *) val)[1];
}
#ifdef WORD64_MAX
void put64_64(void *buff, void *val)
{
   *((word64 *) buff) = *((word64 *) val);
}
void (*put64)(void*, void*) = &put64_64;
#else  /* word32 fallback */
void (*put64)(void*, void*) = &put64_32;
#endif

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

/* Check if buff is all zeros. Returns 1 on true, else 0. */
int iszero_32(void *buff, int len)
{
   word8 *bp = (word8 *) buff;

   for( ; len >= 4; bp += 4, len -= 4) if(*((word32 *) bp)) return 0;
   for( ; len; bp++, len--) if(*bp) return 0;
   return 1;
}  /* end iszero_32() */
#ifdef WORD64_MAX
int iszero_64(void *buff, int len)
{
   word8 *bp = (word8 *) buff;

   for( ; len >= 8; bp += 8, len -= 8) if(*((word64 *) bp)) return 0;
   for( ; len; bp++, len--) if(*bp) return 0;
   return 1;
}  /* end iszero_64() */
int (*iszero)(void*, int) = &iszero_64;
#else  /* word32 fallback */
int (*iszero)(void*, int) = &iszero_32;
#endif

/* 64-bit addition of *ax and *bx. Place result in *cx. 
 * Returns carry. */
int add64_32(void *ax, void *bx, void *cx)
{
   word32 a[2], b[2];
   word32 *c = (word32 *) cx;

   put64(a, ax);
   put64(b, bx);
   c[0] = a[0] + b[0];
   c[1] = a[1] + b[1] + (word32) (c[0] < a[0]);
   return (c[1] < a[1] || (c[1] == a[1] && c[0] < a[0]));
}  /* end add64_32() */
#ifdef WORD64_MAX
int add64_64(void *ax, void *bx, void *cx)
{
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);
   word64 *c = (word64 *) cx;

   *c = a + b;
   return (*c < a);
}
int (*add64)(void*, void*, void*) = &add64_64;
#else  /* word32 fallback */
int (*add64)(void*, void*, void*) = &add64_32;
#endif

/* 64-bit subtraction of *bx from *ax. Place result in *cx.
 * Returns carry. */
int sub64_32(void *ax, void *bx, void *cx)
{  /* 32-bit function variant for sub64() */
   word32 a[2], b[2];
   word32 *c = (word32 *) cx;

   put64(a, ax);
   put64(b, bx);
   c[0] = a[0] - b[0];
   c[1] = a[1] - b[1] - (c[0] > a[0]);
   return (c[1] > a[1] || (c[1] == a[1] && c[0] > a[0]));
}  /* end sub64_word32() */
#ifdef WORD64_MAX  /* 64-bit function variant */
int sub64_64(void *ax, void *bx, void *cx)
{  /* 64-bit function variant for sub64() */
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);
   word64 *c = (word64 *) cx;

   *c = a - b;
   return (*c > a);
}  /* end sub64_word64() */
int (*sub64)(void*, void*, void*) = &sub64_64;
#else  /* ... use 32-bit fallback */
int (*sub64)(void*, void*, void*) = &sub64_32;
#endif  /* end sub64() */

/* Swap sign on 64-bit *ax.
 * NOTE: equivalent to *ax multiplied by -1. */
void negate64_32(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] = ~a[0];
   a[1] = ~a[1];
   if(++a[0] == 0) a[1]++;
}  /* end negate64_32() */
#ifdef WORD64_MAX
void negate64_64(void *ax)
{
   *((word64*) ax) = ~(*((word64*) ax)) + 1;
}  /* end negate64_64() */
void (*negate64)(void*) = &negate64_64;
#else
void (*negate64)(void*) = &negate64_32;
#endif  /* end negate64() */

/* 64-bit unsigned compare *ax to *bx.
 * Returns 1 if *ax > *bx, -1 if *ax < *bx, or 0 if *ax == *bx. */
int cmp64_32(void *ax, void *bx)
{
   word32 *a = (word32 *) ax;
   word32 *b = (word32 *) bx;

   if(a[1] > b[1]) return 1;
   if(a[1] < b[1]) return -1;
   if(a[0] > b[0]) return 1;
   if(a[0] < b[0]) return -1;
   return 0;
}  /* end cmp64_32() */
#ifdef WORD64_MAX
int cmp64_64(void *ax, void *bx)
{
   word64 *a = (word64 *) ax;
   word64 *b = (word64 *) bx;

   if(*a > *b) return 1;
   if(*a < *b) return -1;
   return 0;
}  /* end cmp64_64() */
int (*cmp64)(void*, void*) = &cmp64_64;
#else
int (*cmp64)(void*, void*) = &cmp64_32;
#endif

/* 64-bit shift *ax one to the right. */
void shiftr64_32(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] >>= 1;
   if(a[1] & 1) a[0] |= 0x80000000;
   a[1] >>= 1;
}  /* end shiftr64_32() */
#ifdef WORD64_MAX
void shiftr64_64(void *ax)
{
   *((word64 *) ax) >>= 1;
}  /* end shiftr64_64() */
void (*shiftr64)(void*) = &shiftr64_64;
#else
void (*shiftr64)(void*) = &shiftr64_32;
#endif  /* end shiftr64() */

/* 64-bit multiplication of *ax and *bx. Place result in *cx.
 * Returns 1 if overflow, else 0. */
int mult64_32(void *ax, void *bx, void *cx)
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
}  /* end mult64_32() */
#ifdef WORD64_MAX
int mult64_64(void *ax, void *bx, void *cx)
{
   word64 *c = (word64 *) cx;
   word64 a = *((word64 *) ax);
   word64 b = *((word64 *) bx);

   *c = a * b;
   if (a == 0) return 0; /* avoid division by zero */
   if ((*c / a) == b) return 0; /* check overflow */
   return 1; /* result overflowed */
}  /* end mult64_64() */
int (*mult64)(void*, void*, void*) = &mult64_64;
#else
int (*mult64)(void*, void*, void*) = &mult64_32;
#endif  /* end mult64() */

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
