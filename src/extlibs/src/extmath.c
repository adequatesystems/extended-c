/**
 * @private
 * @headerfile extmath.h <extmath.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

/* include guard */
#ifndef EXTENDED_MATH_C
#define EXTENDED_MATH_C


#include "extmath.h"
#include "extint.h"
#include "extlib.h"


/* x64 guard */
#ifdef HAS_64BIT

   /**
    * Check if `buff[len]` contains all zeros.
    * @returns 1 if @a buff is all zeros, else 0.
    * @note Available ONLY for testing purposes. Use iszero() instead.
   */
   int iszero_x64(void *buff, int len)
   {
      word8 *bp = (word8 *) buff;

      for( ; len >= 8; bp += 8, len -= 8) if(*((word64 *) bp)) return 0;
      for( ; len; bp++, len--) if(*bp) return 0;
      return 1;
   }  /* end iszero_x64() */

   /**
    * 64-bit addition of @a *ax and @a *bx. Result is placed in @a *cx.
    * @returns Resulting carry of operation.
    * @note Available ONLY for testing purposes. Use add64() instead.
   */
   int add64_x64(void *ax, void *bx, void *cx)
   {
      word64 a = *((word64 *) ax);
      word64 b = *((word64 *) bx);
      word64 *c = (word64 *) cx;

      *c = a + b;
      return (*c < a);
   }  /* end add64_x64() */

   /**
    * 64-bit subtraction of @a *bx from @a *ax.
    * Result is placed in @a *cx.
    * @returns Resulting carry of operation.
    * @note Available ONLY for testing purposes. Use sub64() instead.
   */
   int sub64_x64(void *ax, void *bx, void *cx)
   {
      word64 a = *((word64 *) ax);
      word64 b = *((word64 *) bx);
      word64 *c = (word64 *) cx;

      *c = a - b;
      return (*c > a);
   }  /* end sub64_x64() */

   /**
    * Swap sign on 64-bit @a *ax. Equivalent to @a *ax multiplied by (-1).
    * @note Available ONLY for testing purposes. Use negate64() instead.
   */
   void negate64_x64(void *ax)
   {
      *((word64*) ax) = ~(*((word64*) ax)) + 1;
   }  /* end negate64_x64() */

   /**
    * 64-bit unsigned compare @a *ax to @a *bx.
    * @retval -1 if @a *ax < @a *bx
    * @retval 1 if @a *ax > @a *bx
    * @retval 0 if @a *ax == @a *bx.
    * @note Available ONLY for testing purposes. Use cmp64() instead.
   */
   int cmp64_x64(void *ax, void *bx)
   {
      word64 *a = (word64 *) ax;
      word64 *b = (word64 *) bx;

      if(*a < *b) return -1;
      if(*a > *b) return 1;
      return 0;
   }  /* end cmp64_x64() */

   /**
    * 256-bit unsigned compare @a *ax to @a *bx.
    * @retval -1 if @a *ax < @a *bx
    * @retval 1 if @a *ax > @a *bx
    * @retval 0 if @a *ax == @a *bx.
    * @note Available ONLY for testing purposes. Use cmp256() instead.
   */
   int cmp256_x64(void *ax, void *bx)
   {
      word64 *a = (word64 *) ax;
      word64 *b = (word64 *) bx;
      int i;

      for (i = 3; i >= 0; i--) {
         if(a[i] < b[i]) return -1;
         if(a[i] > b[i]) return 1;
      }

      return 0;
   }  /* end cmp256_x64() */

   /**
    * 64-bit shift @a *ax one to the right.
    * @note Available ONLY for testing purposes. Use shiftr64() instead.
   */
   void shiftr64_x64(void *ax)
   {
      *((word64 *) ax) >>= 1;
   }  /* end shiftr64_x64() */

   /**
    * 64-bit multiplication of @a *ax and @a *bx.
    * Result is placed in @a *cx.
    * @result 1 on 64-bit overflow, else 0.
    * @note Available ONLY for testing purposes. Use mult64() instead.
   */
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

/* end x64 guard */
#endif


/**
 * Check if `buff[len]` contains all zeros.
 * @returns 1 if @a buff is all zeros, else 0.
 * @note Available ONLY for testing purposes. Use iszero() instead.
*/
int iszero_x86(void *buff, int len)
{
   word8 *bp = (word8 *) buff;

   for( ; len >= 4; bp += 4, len -= 4) if(*((word32 *) bp)) return 0;
   for( ; len; bp++, len--) if(*bp) return 0;
   return 1;
}  /* end iszero_x86() */

/**
 * 64-bit addition of @a *ax and @a *bx. Result is placed in @a *cx.
 * @returns Resulting carry of operation.
 * @note Available ONLY for testing purposes. Use add64() instead.
*/
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
 * 64-bit subtraction of @a *bx from @a *ax.
 * Result is placed in @a *cx.
 * @returns Resulting carry of operation.
 * @note Available ONLY for testing purposes. Use sub64() instead.
*/
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
 * Swap sign on 64-bit @a *ax. Equivalent to @a *ax multiplied by (-1).
 * @note Available ONLY for testing purposes. Use negate64() instead.
*/
void negate64_x86(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] = ~a[0];
   a[1] = ~a[1];
   if(++a[0] == 0) a[1]++;
}  /* end negate64_x86() */

/**
 * 64-bit unsigned compare @a *ax to @a *bx.
 * @retval -1 if @a *ax < @a *bx
 * @retval 1 if @a *ax > @a *bx
 * @retval 0 if @a *ax == @a *bx.
 * @note Available ONLY for testing purposes. Use cmp64() instead.
*/
int cmp64_x86(void *ax, void *bx)
{
   word32 *a = (word32 *) ax;
   word32 *b = (word32 *) bx;

   if(a[1] < b[1]) return -1;
   if(a[1] > b[1]) return 1;
   if(a[0] < b[0]) return -1;
   if(a[0] > b[0]) return 1;
   return 0;
}  /* end cmp64_x86() */

/**
 * 256-bit unsigned compare @a *ax to @a *bx.
 * @retval -1 if @a *ax < @a *bx
 * @retval 1 if @a *ax > @a *bx
 * @retval 0 if @a *ax == @a *bx.
 * @note Available ONLY for testing purposes. Use cmp256() instead.
*/
int cmp256_x86(void *ax, void *bx)
{
   word32 *a = (word32 *) ax;
   word32 *b = (word32 *) bx;
   int i;

   for (i = 7; i >= 0; i--) {
      if(a[i] < b[i]) return -1;
      if(a[i] > b[i]) return 1;
   }

   return 0;
}  /* end cmp256_x86() */

/**
 * 64-bit shift @a *ax one to the right.
 * @note Available ONLY for testing purposes. Use shiftr64() instead.
*/
void shiftr64_x86(void *ax)
{
   word32 *a = (word32 *) ax;

   a[0] >>= 1;
   if(a[1] & 1) a[0] |= WORD32_C(0x80000000);
   a[1] >>= 1;
}  /* end shiftr64_x86() */

/**
 * 64-bit multiplication of @a *ax and @a *bx.
 * Result is placed in @a *cx.
 * @result 1 on 64-bit overflow, else 0.
 * @note Available ONLY for testing purposes. Use mult64() instead.
*/
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
 * Check if `buff[len]` contains all zeros.
 * @returns 1 if @a buff is all zeros, else 0.
*/
int iszero(void *buff, int len)
{
#ifdef WORD64_MAX
   return iszero_x64(buff, len);
#else
   return iszero_x86(buff, len);
#endif
}  /* end iszero() */

/**
 * 64-bit addition of @a *ax and @a *bx. Result is placed in @a *cx.
 * @returns Resulting carry of operation.
*/
int add64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return add64_x64(ax, bx, cx);
#else
   return add64_x86(ax, bx, cx);
#endif
}  /* end add64() */

/**
 * 64-bit subtraction of @a *bx from @a *ax.
 * Result is placed in @a *cx.
 * @returns Resulting carry of operation.
*/
int sub64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return sub64_x64(ax, bx, cx);

#else
   return sub64_x86(ax, bx, cx);
#endif
}  /* end sub64() */

/**
 * Swap sign on 64-bit @a *ax. Equivalent to @a *ax multiplied by (-1).
*/
void negate64(void *ax)
{
#ifdef WORD64_MAX
   negate64_x64(ax);
#else
   negate64_x86(ax);
#endif
}  /* end negate64() */


/**
 * 64-bit unsigned compare @a *ax to @a *bx.
 * @retval -1 if @a *ax < @a *bx
 * @retval 1 if @a *ax > @a *bx
 * @retval 0 if @a *ax == @a *bx.
*/
int cmp64(void *ax, void *bx)
{
#ifdef WORD64_MAX
   return cmp64_x64(ax, bx);
#else
   return cmp64_x86(ax, bx);
#endif
}  /* end cmp64() */

/**
 * 256-bit unsigned compare @a *ax to @a *bx.
 * @retval -1 if @a *ax < @a *bx
 * @retval 1 if @a *ax > @a *bx
 * @retval 0 if @a *ax == @a *bx.
*/
int cmp256(void *ax, void *bx)
{
#ifdef WORD64_MAX
   return cmp256_x64(ax, bx);
#else
   return cmp256_x86(ax, bx);
#endif
}  /* end cmp256() */

/**
 * 64-bit shift @a *ax one to the right.
*/
void shiftr64(void *ax)
{
#ifdef WORD64_MAX
   shiftr64_x64(ax);
#else
   shiftr64_x86(ax);
#endif
}  /* end shiftr64() */

/**
 * 64-bit multiplication of @a *ax and @a *bx.
 * Result is placed in @a *cx.
*/
int mult64(void *ax, void *bx, void *cx)
{
#ifdef WORD64_MAX
   return mult64_x64(ax, bx, cx);
#else
   return mult64_x86(ax, bx, cx);
#endif
}  /* end mult64() */

/**
 * Multi-byte addition of `ax[bytelen]` and `bx[bytelen]`.
 * Place result in `cx[bytelen]`.
 * @returns Resulting carry of operation.
*/
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

/**
 * Multi-byte subtraction of `bx[bytelen]` from `ax[bytelen]`.
 * Place result in `cx[bytelen]`.
 * @returns Resulting carry of operation.
*/
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

/* end include guard */
#endif
