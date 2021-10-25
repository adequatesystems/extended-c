/**
 * extint.h - Extended integer support header
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 15 September 2018
 * Revised: 25 October 2021
 *
 * Datatypes:
 *    word8  - 1 byte word
 *    word16 - 2 byte word
 *    word32 - 4 byte word
 *    word64 - 8 byte word
 * Number Constants:
 *    WORD8_MAX  - 255
 *    WORD16_MAX - 65535U
 *    WORD32_MAX - 4294967295UL
 *    WORD64_MAX - 18446744073709551615ULL
 *
 * Number literals:
 *    WORD8_C(1)  - 1
 *    WORD16_C(1) - 1U
 *    WORD32_C(1) - 1UL
 *    WORD64_C(1) - 1ULL
 *
 * Printf literals guide:
 * where '#' represents a word's bit width; 8/16/32/64
 *    WORD#d - print value as signed integer
 *    WORD#i - print value as signed integer
 *    WORD#o - print value as octal
 *    WORD#u - print value as unsigned integer
 *    WORD#x - print value as hexadecimal (lowercase)
 *    WORD#X - print value as hexadecimal (UPPERCASE)
 *
 * NOTES:
 * - "emulates" the C Standard Library header file stdint.h,
 *   and "extends" upon the non-standard functionality.
 * - Multi-byte values are assumed little-endian.
 * - All word types are assumed unsigned.
 * Regarding word8...
 * - The word8 datatype supersedes the byte datatype. This is due
 *   due to a historical conflict with windows' BYTE datatype.
 * - A char may also be used in place of a word8, however, care should
 *   be taken to avoid misrepresenting associated char*'s as strings.
 * Regarding word64...
 * - To ensure compilation/compatibility on 32-bit machines, routines
 *   involving word64 should include 32-bit alternatives by testing
 *   for the DISABLE_WORD64 definition at compile time.
 * - Likewise, to disable word64 regardless of availability, define
 *   DISABLE_WORD64 before inclusion or as a compile time definition.
 *
*/

#ifndef _EXTENDED_INTEGER_H_
#define _EXTENDED_INTEGER_H_  /* include guard */

/*
#include <stdint.h>  // extint.h defines WORD#* types using limits.h */
#include <limits.h>

/* ensure system uses 8-bit chars */
#if CHAR_BIT != 8
   Error. Incompatible char bit width.
#endif

/* define type for word8 */
typedef unsigned char word8;
#define WORD8_C(x)            x
#define WORD8_MAX             WORD8_C(0xFF)
#define WORD8_PRINTF_PREFIX   "hh"

/* define type for word16 */
typedef unsigned short int word16;
#define WORD16_C(x)  x ## U
#define WORD16_MAX   WORD16_C(0xFFFF)
#define WORD16_PRINTF_PREFIX  "h"

/* determine type for word32 */
#define WORD32_C(x)  x ## UL
#define WORD32_MAX   WORD32_C(0xFFFFFFFF)
#if ULONG_MAX == WORD32_MAX
   /* long is 32-bit word */
   typedef unsigned long int word32;
   #define WORD32_PRINTF_PREFIX  "l"
#elif UINT_MAX == WORD32_MAX
   /* int is 32-bit word */
   typedef unsigned int word32;
   #define WORD32_PRINTF_PREFIX
#else  /* end #if ULONG_MAX... elif UINT_MAX... */
   Error. Cannot determine type for word32.
#endif  /* end else... */

/* check 64-bit words are not disabled */
#ifndef DISABLE_WORD64
   #define DISABLE_WORD64  /* assume word64 may NOT be available */
   #ifdef ULLONG_MAX  /* assume 64-bit word is available in some form */
      /* we can ONLY rely on ULL to represent our 64-bit max literal */
      #define WORD64_C(x)  x ## ULL
      #define WORD64_MAX   WORD64_C(0xFFFFFFFFFFFFFFFF)
      #if ULLONG_MAX == WORD64_MAX
         /* long long is 64-bit word */
         typedef unsigned long long int word64;
         #define WORD64_PRINTF_PREFIX  "ll"
         #undef DISABLE_WORD64
      #elif ULONG_MAX == WORD64_MAX
         /* long is 64-bit word */
         typedef unsigned long int word64;
         #define WORD64_PRINTF_PREFIX  "l"
         #undef DISABLE_WORD64
      #endif  /* end #if ULLONG_MAX... elif ULONG_MAX... */
   #endif  /* end #ifdef ULLONG_MAX... */
#endif  /* end #ifndef DISABLE_WORD64 */

/* define printf literals for 8-bit word */
#define WORD8d WORD8i
#define WORD8i WORD8_PRINTF_PREFIX  "i"
#define WORD8o WORD8_PRINTF_PREFIX  "o"
#define WORD8u WORD8_PRINTF_PREFIX  "u"
#define WORD8x WORD8_PRINTF_PREFIX  "x"
#define WORD8X WORD8_PRINTF_PREFIX  "X"

/* define printf literals for 16-bit word */
#define WORD16d WORD16i
#define WORD16i WORD16_PRINTF_PREFIX   "i"
#define WORD16o WORD16_PRINTF_PREFIX   "o"
#define WORD16u WORD16_PRINTF_PREFIX   "u"
#define WORD16x WORD16_PRINTF_PREFIX   "x"
#define WORD16X WORD16_PRINTF_PREFIX   "X"

/* define printf literals for 32-bit word */
#define WORD32d WORD32i
#define WORD32i WORD32_PRINTF_PREFIX   "i"
#define WORD32o WORD32_PRINTF_PREFIX   "o"
#define WORD32u WORD32_PRINTF_PREFIX   "u"
#define WORD32x WORD32_PRINTF_PREFIX   "x"
#define WORD32X WORD32_PRINTF_PREFIX   "X"

#ifndef DISABLE_WORD64
   /* define printf literals for 64-bit word */
   #define WORD64d WORD64i
   #define WORD64i WORD64_PRINTF_PREFIX   "i"
   #define WORD64o WORD64_PRINTF_PREFIX   "o"
   #define WORD64u WORD64_PRINTF_PREFIX   "u"
   #define WORD64x WORD64_PRINTF_PREFIX   "x"
   #define WORD64X WORD64_PRINTF_PREFIX   "X"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
void swapbytes(void *val, int n);
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
int iszero(void *buff, int len);
int add64(void *ax, void *bx, void *cx);
int sub64(void *ax, void *bx, void *cx);
void negate64(void *ax);
int cmp64(void *ax, void *bx);
void shiftr64(void *ax);
int mult64(void *ax, void *bx, void *cx);
int multi_add(void *ax, void *bx, void *cx, int bytelen);
int multi_sub(void *ax, void *bx, void *cx, int bytelen);

#ifdef __cplusplus
}
#endif


#endif /* end _EXTENDED_INTEGER_H_ */
