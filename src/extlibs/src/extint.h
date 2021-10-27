/**
 * extint.h - Extended integer support header
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 15 September 2018
 * Revised: 26 October 2021
 *
 * Datatypes:
 *    int8   - 1 byte signed
 *    int16  - 2 byte signed
 *    int32  - 4 byte signed
 *    int64  - 8 byte signed
 *    word8  - 1 byte unsigned
 *    word16 - 2 byte unsigned
 *    word32 - 4 byte unsigned
 *    word64 - 8 byte unsigned
 *
 * Number literals:
 *    INT8_C(1)   - 1
 *    INT16_C(1)  - 1
 *    INT32_C(1)  - 1L
 *    INT64_C(1)  - 1LL
 *    WORD8_C(1)  - 1
 *    WORD16_C(1) - 1U
 *    WORD32_C(1) - 1UL
 *    WORD64_C(1) - 1ULL
 *
 * Number Constants:
 *    INT8_MIN   - (-128)
 *    INT16_MIN  - (-32768)
 *    INT32_MIN  - (-12147483648L)
 *    INT64_MIN  - (-9223372036854775808LL)
 *    INT8_MAX   - 127
 *    INT16_MAX  - 32767
 *    INT32_MAX  - 2147483647L
 *    INT64_MAX  - 9223372036854775807LL
 *    WORD8_MAX  - 255
 *    WORD16_MAX - 65535U
 *    WORD32_MAX - 4294967295UL
 *    WORD64_MAX - 18446744073709551615ULL
 *
 * Printf literals guide:
 * where '#' represents bit width; 8/16/32/64
 *    PRId# - print value as a signed integer
 *    PRIi# - print value as a signed integer
 *    PRIo# - print value as an octal
 *    PRIu# - print value as an unsigned integer
 *    PRIx# - print value as a hexadecimal (lowercase)
 *    PRIX# - print value as a hexadecimal (UPPERCASE)
 *
 * NOTES:
 * - "emulates" the C Standard Library header file stdint.h,
 *   and "extends" upon the non-standard functionality.
 * - Multi-byte values are assumed little-endian.
 * - All word types are assumed unsigned.
 * - All int types are assumed signed.
 * Regarding word8...
 * - The word8 datatype supersedes the byte datatype. This is due to
 *   a historical conflict with the BYTE datatype in the Windows API.
 * - A char may also be used in place of a word8, however, care should
 *   be taken to avoid misrepresenting associated char*'s as strings.
 * Regarding word64 and int64...
 * - To ensure compilation/compatibility on 32-bit machines, routines
 *   involving word64 should include 32-bit alternatives by testing
 *   for the WORD64 definition using #ifdef.
 * - To disable word64 and int64, regardless of availability, define
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

/* define 8-bit types */
#define INT8_C(x)       x
#define WORD8_C(x)      x
#define INT8_MIN        INT8_C(0x80)
#define INT8_MAX        INT8_C(0x7F)
#define WORD8_MAX       WORD8_C(0xFF)
#define PRI8_PREFIX     "hh"
typedef char int8;
typedef unsigned char word8;

/* define 16-bit types */
#define INT16_C(x)      x
#define WORD16_C(x)     x ## U
#define INT16_MIN       INT16_C(0x8000)
#define INT16_MAX       INT16_C(0x7FFF)
#define WORD16_MAX      WORD16_C(0xFFFF)
#define PRI16_PREFIX    "h"
typedef short int int16;
typedef unsigned short int word16;

/* determine type for word32 */
#define INT32_C(x)      x ## L
#define WORD32_C(x)     x ## UL
#define INT32_MIN       INT32_C(0x80000000)
#define INT32_MAX       INT32_C(0x7FFFFFFF)
#define WORD32_MAX      WORD32_C(0xFFFFFFFF)
#if UINT_MAX == WORD32_MAX  /* int is preferred 32-bit word */
   #define PRI32_PREFIX
   typedef int int32;
   typedef unsigned int word32;
#elif ULONG_MAX == WORD32_MAX  /* long is preferred 32-bit word */
   #define PRI32_PREFIX "l"
   typedef long int int32;
   typedef unsigned long int word32;
#else  /* end #if ULONG_MAX... elif UINT_MAX... */
   Error. Cannot determine type for word32.
#endif  /* end else... */

/* check 64-bit words are not disabled */
#ifndef DISABLE_WORD64
   #ifdef ULLONG_MAX  /* assume 64-bit word is available in some form */
      /* we can ONLY rely on ULL to represent our 64-bit max literal */
      #define INT64_C(x)   x ## LL
      #define WORD64_C(x)  x ## ULL
      #define INT64_MAX    INT64_C(0x7FFFFFFFFFFFFFFF)
      #define INT64_MIN    INT64_C(0x8000000000000000)
      #define WORD64_MAX   WORD64_C(0xFFFFFFFFFFFFFFFF)
      #if ULONG_MAX == WORD64_MAX  /* long is preferred 64-bit word */
         #define WORD64  /* definition to indicate availability */
         #define PRI64_PREFIX "l"
         typedef long int int64;
         typedef unsigned long int word64;
      #elif ULLONG_MAX == WORD64_MAX  /* long long preferred is 64-bit word */
         #define WORD64  /* definition to indicate availability */
         #define PRI64_PREFIX "ll"
         typedef long long int int64;
         typedef unsigned long long int word64;
      #endif  /* end #if ULLONG_MAX... elif ULONG_MAX... */
   #endif  /* end #ifdef ULLONG_MAX... */
#endif  /* end #ifndef DISABLE_WORD64 */

/* define printf literals for 8-bit types */
#define PRId8       PRI8_PREFIX "d"
#define PRIi8       PRI8_PREFIX "i"
#define PRIo8       PRI8_PREFIX "o"
#define PRIu8       PRI8_PREFIX "u"
#define PRIx8       PRI8_PREFIX "x"
#define PRIX8       PRI8_PREFIX "X"

/* define printf literals for 16-bit types */
#define PRId16      PRI16_PREFIX "d"
#define PRIi16      PRI16_PREFIX "i"
#define PRIo16      PRI16_PREFIX "o"
#define PRIu16      PRI16_PREFIX "u"
#define PRIx16      PRI16_PREFIX "x"
#define PRIX16      PRI16_PREFIX "X"

/* define printf literals for 32-bit types */
#define PRId32      PRI32_PREFIX "d"
#define PRIi32      PRI32_PREFIX "i"
#define PRIo32      PRI32_PREFIX "o"
#define PRIu32      PRI32_PREFIX "u"
#define PRIx32      PRI32_PREFIX "x"
#define PRIX32      PRI32_PREFIX "X"

#ifdef WORD64
   /* define printf literals for 64-bit types */
   #define PRId64   PRI64_PREFIX "d"
   #define PRIi64   PRI64_PREFIX "i"
   #define PRIo64   PRI64_PREFIX "o"
   #define PRIu64   PRI64_PREFIX "u"
   #define PRIx64   PRI64_PREFIX "x"
   #define PRIX64   PRI64_PREFIX "X"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes */
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
