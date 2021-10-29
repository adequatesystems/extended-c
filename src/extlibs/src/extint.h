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
 * Number Constants:
 *    WORD8_MAX  - 255
 *    WORD16_MAX - 65535U
 *    WORD32_MAX - 4294967295UL
 *    WORD64_MAX - 18446744073709551615ULL
 * Number Constants on signed equivalents have been removed
 * due to a lack of determining a naming scheme that does not
 * conflict with the C Standard Library header, "stdint.h".
 * If Number Constants on signed equivalents are required, use
 * exact number literals, or define the following arithmetic:
 * where '#' represents bit width; 8/16/32/64
 *    INT#_MAX = ( WORD#_MAX / 2 )
 *    INT#_MIN = ( ( WORD#_MAX / (-2) ) - 1)
 *
 * Printf literals guide:
 * where '#' represents bit width; 8/16/32/64
 *    INTd# - print signed value as a string
 *    INTi# - print signed value as a string
 *    INTo# - print signed value as an octal string
 *    INTx# - print signed value as a hexadecimal string (lowercase)
 *    INTX# - print signed value as a hexadecimal string (UPPERCASE)
 *    WORDo# - print unsigned value as an octal string
 *    WORDu# - print unsigned value as a string
 *    WORDx# - print unsigned value as a hexadecimal string (lowercase)
 *    WORDX# - print unsigned value as a hexadecimal string (UPPERCASE)
 *
 * NOTES:
 * - "emulates" the C Standard Library header file stdint.h,
 *   and "extends" upon the non-standard functionality.
 * - For most functions, Multi-byte values are assumed little-endian.
 * - All word# types are unsigned, and all int# types are signed.
 * Regarding word8...
 * - The word8 datatype serves to supersede the byte datatype, due to a
 *   redefinition conflict with the std::byte type introduced in C++17.
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
#include <stdint.h>  // datatypes are defined using limits.h instead */
#include <limits.h>

/* ensure system uses 8-bit chars */
#if CHAR_BIT != 8
   Error. Incompatible char bit width.
#endif

/* define 8-bit types */
#define WORD8_C(x)      x  /* no number literal required */
#define WORD8_MAX       WORD8_C(0xFF)
#define PRI8_PREFIX     "hh"
typedef char int8;
typedef unsigned char word8;

/* define 16-bit types */
#define WORD16_C(x)     x ## U  /* assume 16-bit literal */
#define WORD16_MAX      WORD16_C(0xFFFF)
#if UINT_MAX == WORD16_MAX  /* int is preferred */
   #define PRI16_PREFIX
   typedef int int16;
   typedef unsigned int word16;
#elif USHRT_MAX == WORD16_MAX  /* short is preferred */
   #undef WORD16_C  /* redefine actual 16-bit literal */
   #undef WORD16_MAX  /* redefine actual 16-bit max */
   #define WORD16_C(x)  x ## UL
   #define WORD16_MAX   WORD16_C(0xFFFF)
   #define PRI16_PREFIX "h"
   typedef short int int16;
   typedef unsigned short int word16;
#else  /* end #if ULONG_MAX... elif UINT_MAX... */
   Error. Cannot determine type for word16.
#endif  /* end else... */

/* determine type for word32 */
#define WORD32_C(x)     x ## UL  /* assume 32-bit literal */
#define WORD32_MAX      WORD32_C(0xFFFFFFFF)
#if ULONG_MAX == WORD32_MAX  /* long is preferred */
   #define PRI32_PREFIX "l"
   typedef long int int32;
   typedef unsigned long int word32;
#elif UINT_MAX == WORD32_MAX  /* int is preferred */
   #undef WORD32_C  /* redefine actual 32-bit literal */
   #undef WORD32_MAX  /* redefine actual 32-bit max */
   #define WORD32_C(x)  x ## UL
   #define WORD32_MAX   WORD32_C(0xFFFFFFFF)
   #define PRI32_PREFIX
   typedef int int32;
   typedef unsigned int word32;
#else  /* end #if ULONG_MAX... elif UINT_MAX... */
   Error. Cannot determine type for word32.
#endif  /* end else... */

/* check 64-bit words are not disabled */
#ifndef DISABLE_WORD64
   #ifdef ULLONG_MAX  /* assume 64-bit word is available in some form */
      #define WORD64_C(x)     x ## ULL  /* assume 64-bit literal */
      #define WORD64_MAX      WORD64_C(0xFFFFFFFFFFFFFFFF)
      #if ULLONG_MAX == WORD64_MAX  /* long long is preferred */
         #define WORD64  /* definition to indicate availability */
         #define PRI64_PREFIX "ll"
         typedef long long int int64;
         typedef unsigned long long int word64;
      #elif ULONG_MAX == WORD64_MAX  /* long is preferred */
         #define WORD64  /* definition to indicate availability */
         #undef WORD64_C  /* redefine actual 64-bit literal */
         #undef WORD64_MAX  /* redefine actual 64-bit max */
         #define WORD64_C(x)  x ## UL
         #define WORD64_MAX   WORD64_C(0xFFFFFFFFFFFFFFFF)
         #define PRI64_PREFIX "l"
         typedef long int int64;
         typedef unsigned long int word64;
      #endif  /* end #if ULLONG_MAX... elif ULONG_MAX... */
   #endif  /* end #ifdef ULLONG_MAX... */
#endif  /* end #ifndef DISABLE_WORD64 */

/* define printf literals for 8-bit types */
#define INTd8        PRI8_PREFIX "d"
#define INTi8        PRI8_PREFIX "i"
#define INTo8        PRI8_PREFIX "o"
#define INTx8        PRI8_PREFIX "x"
#define INTX8        PRI8_PREFIX "X"
#define WORDo8       PRI8_PREFIX "o"
#define WORDu8       PRI8_PREFIX "u"
#define WORDx8       PRI8_PREFIX "x"
#define WORDX8       PRI8_PREFIX "X"

/* define printf literals for 16-bit types */
#define INTd16       PRI16_PREFIX "d"
#define INTi16       PRI16_PREFIX "i"
#define INTo16       PRI16_PREFIX "o"
#define INTx16       PRI16_PREFIX "x"
#define INTX16       PRI16_PREFIX "X"
#define WORDo16      PRI16_PREFIX "o"
#define WORDu16      PRI16_PREFIX "u"
#define WORDx16      PRI16_PREFIX "x"
#define WORDX16      PRI16_PREFIX "X"

/* define printf literals for 32-bit types */
#define INTd32       PRI32_PREFIX "d"
#define INTi32       PRI32_PREFIX "i"
#define INTo32       PRI32_PREFIX "o"
#define INTx32       PRI32_PREFIX "x"
#define INTX32       PRI32_PREFIX "X"
#define WORDo32      PRI32_PREFIX "o"
#define WORDu32      PRI32_PREFIX "u"
#define WORDx32      PRI32_PREFIX "x"
#define WORDX32      PRI32_PREFIX "X"

#ifdef WORD64
   /* define printf literals for 64-bit types */
   #define INTd64    PRI64_PREFIX "d"
   #define INTi64    PRI64_PREFIX "i"
   #define INTo64    PRI64_PREFIX "o"
   #define INTx64    PRI64_PREFIX "x"
   #define INTX64    PRI64_PREFIX "X"
   #define WORDo64   PRI64_PREFIX "o"
   #define WORDu64   PRI64_PREFIX "u"
   #define WORDx64   PRI64_PREFIX "x"
   #define WORDX64   PRI64_PREFIX "X"
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
