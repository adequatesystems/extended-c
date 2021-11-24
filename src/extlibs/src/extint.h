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
 *   for the WORD64_MAX definition using #ifdef.
 * - To disable word64 and int64, regardless of availability, define
 *   DISABLE_WORD64 before inclusion or as a compile time definition.
 *
*/

#ifndef EXTENDED_INTEGER_H
#define EXTENDED_INTEGER_H  /* include guard */


/*
#include <stdint.h>  // datatypes are defined using limits.h instead */
#include <limits.h>


/* 8-BIT DATATYPES
 * ensure system compatibility with 8-bit chars */
#if CHAR_BIT == 8

   /* char is preferred 8-bit word */
   typedef char            int8;
   typedef unsigned char   word8;
   #define WORD8_MAX       0xFF
   #define WORD8_C(x)      x
   #define PRI8_PREFIX     "hh"

#else  /* end #if CHAR_BIT... */
   #error Incompatible char bit width.
#endif

/* define printf literals for 8-bit datatypes */
#define INTd8     PRI8_PREFIX "d"
#define INTi8     PRI8_PREFIX "i"
#define INTo8     PRI8_PREFIX "o"
#define INTx8     PRI8_PREFIX "x"
#define INTX8     PRI8_PREFIX "X"
#define WORDo8    PRI8_PREFIX "o"
#define WORDu8    PRI8_PREFIX "u"
#define WORDx8    PRI8_PREFIX "x"
#define WORDX8    PRI8_PREFIX "X"


/* 16-BIT DATATYPES
 * determine best datatype with 16-bit width */
#if UINT_MAX == 0xFFFFU

   /* int is preferred 16-bit word */
   typedef int                int16;
   typedef unsigned int       word16;
   #define WORD16_MAX         0xFFFFU
   #define WORD16_C(x)        x ## U
   #define PRI16_PREFIX

#elif USHRT_MAX == 0xFFFF

   /* short is preferred 16-bit word */
   typedef short int          int16;
   typedef unsigned short int word16;
   #define WORD16_MAX         0xFFFF
   #define WORD16_C(x)        x
   #define PRI16_PREFIX       "h"

#else  /* end #if UINT_MAX... #elif USHRT_MAX... */
   #error Cannot determine datatype for 16-bit word.
#endif

/* define printf literals for 16-bit datatypes */
#define INTd16    PRI16_PREFIX "d"
#define INTi16    PRI16_PREFIX "i"
#define INTo16    PRI16_PREFIX "o"
#define INTx16    PRI16_PREFIX "x"
#define INTX16    PRI16_PREFIX "X"
#define WORDo16   PRI16_PREFIX "o"
#define WORDu16   PRI16_PREFIX "u"
#define WORDx16   PRI16_PREFIX "x"
#define WORDX16   PRI16_PREFIX "X"


/* 32-BIT DATATYPES
 * determine best datatype with 32-bit width */
#if ULONG_MAX == 0xFFFFFFFFUL

   /* long is preferred 32-bit word */
   typedef long int           int32;
   typedef unsigned long int  word32;
   #define WORD32_MAX         0xFFFFFFFFUL
   #define WORD32_C(x)        x ## UL
   #define PRI32_PREFIX       "l"

#elif UINT_MAX == 0xFFFFFFFFU

   /* int is preferred 32-bit word */
   typedef int                int32;
   typedef unsigned int       word32;
   #define WORD32_MAX         0xFFFFFFFFU
   #define WORD32_C(x)        x
   #define PRI32_PREFIX

#else  /* end #if ULONG_MAX... elif UINT_MAX... */
   #error Cannot determine datatype for 32-bit word.
#endif

/* define printf literals for 32-bit datatypes */
#define INTd32    PRI32_PREFIX "d"
#define INTi32    PRI32_PREFIX "i"
#define INTo32    PRI32_PREFIX "o"
#define INTx32    PRI32_PREFIX "x"
#define INTX32    PRI32_PREFIX "X"
#define WORDo32   PRI32_PREFIX "o"
#define WORDu32   PRI32_PREFIX "u"
#define WORDx32   PRI32_PREFIX "x"
#define WORDX32   PRI32_PREFIX "X"


/* 64-BIT DATATYPES
 * Ensure 64-bit words are not disabled (DISABLE_WORD64)
 * Ensure 64-bit words are available to the system (ULLONG_MAX) */
#if ! defined(DISABLE_WORD64) && defined(ULLONG_MAX)

   /* determine best datatype for 64-bit word */
   #if ULLONG_MAX == 0xFFFFFFFFFFFFFFFFULL

      /* long long is preferred 64-bit word */
      typedef long long int            int64;
      typedef unsigned long long int   word64;
      #define WORD64_MAX               0xFFFFFFFFFFFFFFFFULL
      #define WORD64_C(x)              x ## ULL
      #define PRI64_PREFIX             "ll"

   #elif ULONG_MAX == 0xFFFFFFFFFFFFFFFFUL

      /* long is preferred 64-bit word */
      typedef long int                 int64;
      typedef unsigned long int        word64;
      #define WORD64_MAX               0xFFFFFFFFFFFFFFFFUL
      #define WORD64_C(x)              x ## UL
      #define PRI64_PREFIX             "l"

   #else  /* end #if ULLONG_MAX... elif ULONG_MAX... */
      #error Cannot determine datatype for 64-bit word.
   #endif

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

#endif  /* end #ifndef DISABLE_WORD64 */


#endif /* end _EXTENDED_INTEGER_H_ */
