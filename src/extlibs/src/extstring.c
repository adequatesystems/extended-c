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
#include "extint.h"     /* for rand16fast() */

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

/* Shuffle a list[count] of size byte elements using Durstenfeld's
 * implementation of the Fisher-Yates shuffling algorithm.
 * NOTES:
 * - shuffle count is bound to 16 bits due to rand16fast() modulo
 * - seed PRNG before use with srand16fast(seed) */
void shuffle(void *list, size_t size, size_t count)
{
   unsigned char *listp = (unsigned char *) list;
   unsigned char *elemp, *swapp;

   if (count < 2) return;  /* list is not worth shuffling */
   elemp = &listp[(count - 1) * size];
   for( ; count > 1; count--, elemp -= size) {
      /* for every element (in reverse order), swap with random
       * element whose index is less than the current */
      swapp = &listp[(rand16fast() % count) * size];
      memswap(elemp, swapp, size);
   }
}  /* end shuffle() */


#endif /* end _EXTENDED_STRING_C_ */
