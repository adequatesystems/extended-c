/**
 * extstring.c - Extended string handling support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 2 January 2018
 * Revised: 25 October 2021
 *
*/

#ifndef _EXTENDED_STRING_C_
#define _EXTENDED_STRING_C_  /* include guard */


#include "extstring.h"  /* header file */

#define MEMSWAPCHUNK 1024  /* 1 KiB temp resource for swapping memory */

/* Swaps count characters between objects pointed to by ax and bx.
 * Swap occurs in 1 kibibyte (MEMSWAPCHUNK) chunks.
 * NOTES:
 * - For efficacy validation, see https://godbolt.org/z/Wo4K4zWcP */
void memswap(void *ax, void *bx, size_t count)
{
   unsigned char *pa = (unsigned char *) ax;
   unsigned char *pb = (unsigned char *) bx;
   unsigned char temp[MEMSWAPCHUNK];

   while (count >= MEMSWAPCHUNK) {
      memcpy(temp, pa, MEMSWAPCHUNK);
      memcpy(pa, pb, MEMSWAPCHUNK);
      memcpy(pb, temp, MEMSWAPCHUNK);
      count -= MEMSWAPCHUNK;
      pa += MEMSWAPCHUNK;
      pb += MEMSWAPCHUNK;
   }
   if (count) {
      memcpy(temp, pa, count);
      memcpy(pa, pb, count);
      memcpy(pb, temp, count);
   }
}  /* end memswap() */


#endif /* end _EXTENDED_STRING_C_ */
