/**
 * @private
 * @headerfile extstring.h <extstring.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_STRING_C
#define EXTENDED_STRING_C


#include "extstring.h"

/* internal support */
#include "exterrno.h"

/* external support */
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
   #include "win32lean.h"

#endif

/**
 * Swap @a count bytes between pointers @a ax and @a bx.
 * Swap occurs in, at most, BUFSIZ chunks.
 * @param ax Pointer to primary array of bytes to swap
 * @param bx Pointer to secondary array of bytes to swap
*/
void memswap(void *ax, void *bx, size_t count)
{
   unsigned char *pa = (unsigned char *) ax;
   unsigned char *pb = (unsigned char *) bx;
   unsigned char temp[BUFSIZ];

   while (count >= BUFSIZ) {
      memcpy(temp, pa, BUFSIZ);
      memcpy(pa, pb, BUFSIZ);
      memcpy(pb, temp, BUFSIZ);
      count -= BUFSIZ;
      pa += BUFSIZ;
      pb += BUFSIZ;
   }
   if (count) {
      memcpy(temp, pa, count);
      memcpy(pa, pb, count);
      memcpy(pb, temp, count);
   }
}  /* end memswap() */

/**
 * Get a textual description of an "extended-c" module error code.
 * The error code passed to this function must be an error code,
 * that was set or returned by one of the "extended-c" functions.
 * @param errnum Value of the "extended-c" error number
 * @return (char *) to a textual description of error
*/
char *strerror_ext(int errnum, char *buf, size_t buflen)
{
#ifdef _WIN32
   if (errnum < 0) {
      FormatMessageA(
         FORMAT_MESSAGE_ALLOCATE_BUFFER |
         FORMAT_MESSAGE_FROM_SYSTEM |
         FORMAT_MESSAGE_IGNORE_INSERTS,
         NULL, (DWORD) -(errnum), 0, (LPSTR) buf, 0, NULL);
      /* NOTE: -(errnum): Windows system error codes are applied to errno
       * as negative values to avoid conflict with POSIX error codes. */
      return buf;
   }

   /* fallback to Windows' strerror_s() */
   strerror_s(buf, buflen, errnum);

/* end Windows */
#elif (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && ! _GNU_SOURCE
   /* ^ XSI-compliant strerror_r() feature test */
   strerror_r(errnum, buf, buflen);

/* end XSI-compliant strerror_r() */
#else  /* use GNU strerror_r() fallback */
   /* NOTE: GNU's strerror_r() MAY NOT USE the provided buffer, so copy */
   strncpy(buf, strerror_r(errnum, buf, buflen), buflen)
   /* ensure buf is null terminated */
   if (buflen > 0 && buf[buflen - 1]) buf[buflen - 1] = '\0';

/* end GNU strerror_r() */
#endif

   /* return buf */
   return buf;
}

/* end include guard */
#endif
