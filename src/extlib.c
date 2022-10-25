/**
 * @private
 * @headerfile extlib.h <extlib.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_UTILITIES_C
#define EXTENDED_UTILITIES_C


#include "extlib.h"

/* internal support */
#include "exterrno.h"
#include "extio.h"      /* for f*64() functions in filesort */
#include "extmath.h"    /* for iszero() in *nz() functions */
#include "extstring.h"  /* for memory manipulation support */

/* Internal state seeds for PRNG's */
static word32 Lseed = 1;
static word32 Lseed2 = 1;
static word32 Lseed3 = 362436069;
static word32 Lseed4 = 123456789;
static word32 State128[4] =
   { 0xcafef00d, 0xf01dab1e, 0x5eed1e55, 0x1dea112e };

#ifndef rotl
   #define rotl(x, n)  ( ((x) << (n)) | ((x) >> (32 - (n))) )

#endif

/* 64-bit guard */
#ifdef HAS_64BIT

   /* Place a 64-bit unsigned @a value in @a buff. */
   void put64_x64(void *buff, void *value)
   {
      *((word64 *) buff) = *((word64 *) value);
   }

#endif  /* end WORD64_MAX */

/* Place a 64-bit unsigned @a value in @a buff. */
void put64_x86(void *buff, void *value)
{
   ((word32 *) buff)[0] = ((word32 *) value)[0];
   ((word32 *) buff)[1] = ((word32 *) value)[1];
}

/**
 * Get a 16-bit unsigned value from @a buff.
 * @param buff Pointer to buffer to get value from
 * @returns 16-bit unsigned value from @a buff.
*/
word16 get16(void *buff)
{
   return *((word16 *) buff);
}

/**
 * Place a 16-bit unsigned @a value in @a buff.
 * @param buff Pointer to buffer to place value
 * @param value 16-bit unsigned value
*/
void put16(void *buff, word16 value)
{
   *((word16 *) buff) = value;
}

/**
 * Get a 32-bit unsigned value from @a buff.
 * @param buff Pointer to buffer to get value from
 * @returns 32-bit unsigned value from @a buff.
*/
word32 get32(void *buff)
{
   return *((word32 *) buff);
}

/**
 * Place a 32-bit @a value in @a buff.
 * @param buff Pointer to buffer to place value
*/
void put32(void *buff, word32 value)
{
   *((word32 *) buff) = value;
}

/**
 * Place a 64-bit unsigned @a value in @a buff.
 * @param buff Pointer to buffer to place value
 * @param value Pointer to 64-bit value
*/
void put64(void *buff, void *value)
{
#ifdef WORD64_MAX
   put64_x64(buff, value);
#else
   put64_x86(buff, value);
#endif
}

/**
 * Set the internal state seed for rand16fast().
 * @param x Value to set the internal state seed @a Lseed to.
*/
void srand16fast(word32 x)
{
   Lseed = x;
}

/**
 * Get the current internal state seed used by rand16fast().
 * @returns Value of internal state seed @a Lseed. */
word32 get_rand16fast(void)
{
   return Lseed;
}

/**
 * Set the internal state seed for rand16().
 * @param x Value to set the internal state seed @a Lseed to
 * @param y Value to set the internal state seed @a Lseed to
 * @param z Value to set the internal state seed @a Lseed to
*/
void srand16(word32 x, word32 y, word32 z)
{
   Lseed2 = x;
   Lseed3 = y;
   Lseed4 = z;
}

/**
 * Get the current internal state seeds used by rand16().
 * @param x Pointer to location to place internal state seed @a Lseed2
 * @param y Pointer to location to place internal state seed @a Lseed3
 * @param z Pointer to location to place internal state seed @a Lseed4
*/
void get_rand16(word32 *x, word32 *y, word32 *z)
{
   *x = Lseed2;
   *y = Lseed3;
   *z = Lseed4;
}

/**
 * Fast 16-bit PRNG using internal state seed @a Lseed.
 * Based on Dr. Marsaglia's Usenet post of a linear
 * congruential generator.
 * @returns Random number range [0, 65535].
 * @warning Exhibits an increase in PRNG generation at the
 * cost of statistical randomness, when compared to rand16().
*/
word32 rand16fast(void)
{
   Lseed = Lseed * WORD32_C(69069) + WORD32_C(262145);

   return Lseed >> 16;
}

/**
 * 16-bit PRNG using internal state seeds @a Lseed2/3/4.
 * Based on Dr. Marsaglia's KISS method. Produces
 * reasonable 16-bit statistical randomness.
 * @returns Random number range [0, 65535].
*/
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
   /* the KISS method (combination of methods) */
   return (Lseed2 ^ (Lseed3 << 16) ^ Lseed4) >> 16;
}  /* end rand16() */

/**
 * 32-bit PRNG using 128-bits of internal state seeds.
 * Based on Xoshiro128** by David Blackman and Sebastiano Vigna.
 * @returns (word32) value representing a random 32-bit unsigned integer.
*/
word32 rand32(void)
{
	const word32 result = rotl(State128[1] * 5, 7) * 9;
	const word32 t = State128[1] << 9;

	State128[2] ^= State128[0];
	State128[3] ^= State128[1];
	State128[1] ^= State128[2];
	State128[0] ^= State128[3];

	State128[2] ^= t;

	State128[3] = rotl(State128[3], 11);

	return result;
}  /* end rand32() */

/**
 * Generate an internal state seed for rand32(), using a value.
 * State generation based on SplitMix64 by Sebastiano Vigna.
 * @param x 64-bit unsigned integer value to seed rand32() with
*/
void srand32(unsigned long long x)
{
   unsigned long long z;
   z = (x += WORD64_C(0x9e3779b97f4a7c15));
	z = (z ^ (z >> 30)) * WORD64_C(0xbf58476d1ce4e5b9);
	z = (z ^ (z >> 27)) * WORD64_C(0x94d049bb133111eb);
	*((unsigned long long *) State128) = z ^ (z >> 31);
}  /* end srand32() */

/**
 * Shuffle a `list[count]` of @a size byte elements.
 * Uses Durstenfeld's implementation of the Fisher-Yates
 * shuffling algorithm.
 * @note Shuffle @a count is bound to 16 bits due to rand16().
 * @note Set random seed with srand16() before use.
*/
void shuffle(void *list, size_t size, size_t count)
{
   unsigned char *listp = (unsigned char *) list;
   unsigned char *elemp, *swapp;

   if (count < 2) return;  /* list is not worth shuffling */
   elemp = &listp[(count - 1) * size];
   for( ; count > 1; count--, elemp -= size) {
      /* for every element (in reverse order), swap with random
       * element whose index is less than the current */
      swapp = &listp[(rand16() % count) * size];
      memswap(elemp, swapp, size);
   }
}  /* end shuffle() */

/**
 * Shuffle a `list[count]` of non-zero, @a size byte elements.
 * A zero value marks the end of list, and shuffling does
 * NOT occur for said value onwards.<br/>Uses Durstenfeld's
 * implementation of the Fisher-Yates shuffling algorithm.
 * @note Shuffle @a count is bound to 16 bits due to rand16().
 * @note Set random seed with srand16() before use.
*/
void shufflenz(void *list, size_t size, size_t count)
{
   char *listp = ((char *) list) + (size * count) - size;

   /* reduce count for every zero value at the end of the list */
   for( ; count > 0 && iszero(listp, size); listp -= size, count--);
   shuffle(list, size, count);
}  /* end shufflenz() */

/* Search a `list[count]` of non-zero, @a size byte elements
 * for a @a value.
 * @returns Pointer to found value, else NULL if not found.
*
void *search(void *value, void *list, size_t size, size_t count)
{
   unsigned char listp = (unsigned char *) list;

   for( ; count--; listp += size) {
      if(memcmp(value, listp, size)) continue;
      return ((void *) listp);
   }

   return NULL;
}  // end search() */

/* Search a `list[count]` of non-zero, @a size byte elements
 * for a @a value. A zero value marks the end of list.
 * @returns Pointer to found value, else NULL if not found.
*
void *searchnz(void *value, void *list, size_t size, size_t count)
{
   unsigned char listp = (unsigned char *) list;

   for( ; count-- && iszero(listp, size); listp += size) {
      if(memcmp(value, listp, size)) continue;
      return ((void *) listp);
   }

   return NULL;
}  // end searchnz() */

/* Extract a @a value from a `list[count]` of non-zero, @a size
 * byte elements.
 * @returns Pointer to extracted value, else NULL if not found.
*
void *extractnz(void *value, void *list, size_t size, size_t count)
{
   char *end = &((char *) list)[size * count];
   char *last = end - size;
   void *found;

   found = searchnz(value, list, size, count);
   if (found == NULL) return NULL;
   if (found < ((size_t) end - size)) {
      memmove(found, ((char *) found)[size], (size_t) (end - found));
   }  // set final element zero
   memset(last, 0, size);

   return value;
}  // end extractnz() */

/* Append a non-zero, @a size byte value to a `list[count]`.
 * @returns Pointer to appended value, else NULL if not appended.
*
void *appendnz(void *value, void *list, size_t size, size_t count)
{
   char *listp = (char *) list;

   if (value == NULL || iszero(value, size)) return NULL;
   for( ; listp < &listp[size * count]; listp += size) {
      if (iszero(listp, size)) {
         memset(listp, value, size);
         return ((void *) listp);
      }
   }

   return NULL;
}  // end appendnz() */

/**
 * Perform a binary search for @a len bytes of @a key in @a ptr.
 * Data at @a ptr is expected to be sorted in blocks of @a len bytes.
 * Performance comparison of the C Standard bsearch() function
 * is negligible... <https://godbolt.org/z/TKKjcc6ev>
 * @param key Pointer to key to search for
 * @param len Length, in bytes, of key to compare
 * @param ptr Pointer to data to search in
 * @param count Number of elements to search
 * @param size Number of bytes of each element
 * @returns (void *) Pointer to found element, or NULL if not found.
*/
void *bsearch_len(const void *key, size_t len,
   const void *ptr, size_t count, size_t size)
{
   size_t mid, hi, lo;
   void *data;
   int cond;

   /* init */
   hi = count - 1;
   lo = 0;

   /* binary search ptr for key */
   while (lo <= hi) {
      mid = (hi + lo) / 2;
      data = (char *) ptr + (mid * size);
      cond = memcmp(key, (char *) data, len);
      /* adjust and repeat -- find first occurrence */
      if (cond == 0) return data;
      if (lo == hi) break;
      if (cond < 0) hi = mid - 1; else lo = mid + 1;
   }  /* end while */

   return NULL;
}  /* end bsearch_len() */

/**
 * Sort a file containing @a size length elements. If file data fits into
 * the memory buffer, @a bufsz, data is simply sorted in-memory with quick
 * sort. Otherwise, an external merge sort algorithm is applied.
 * @param filename Name of file to sort
 * @param size Size of each element in file
 * @param bufsz Size of the buffer of each run used for in-memory sorting
 * @param comp Comparison function to use when sorting elements
 * @returns 0 on success, or non-zero on error. Check errno for details.
 * @exception errno=EINVAL A function parameter is invalid
*/
int filesort(const char *filename, size_t size, size_t blocksz,
   int (*comp)(const void *, const void *))
{
   void *a, *b, *buffer;
   FILE *afp, *bfp, *ofp;
   long long aidx, bidx;
   long long filelen, block;
   long long start, mid, end;
   size_t filecount, count, in;
   int cond;
   char fname[FILENAME_MAX];

   /* sanity checks */
   if (filename == NULL || comp == NULL) goto FAIL_INVAL;
   if (size == 0 || blocksz == 0) goto FAIL_INVAL;

   /* PHASE 1: pre-sort blocks of data */

   /* get count for blocksz (adjust) */
   count = blocksz / size;
   blocksz = count * size;
   /* create buffer, open input/output files */
   ofp = fopen(filename, "rb+");
   buffer = malloc(blocksz);
   /* check failures */
   if (ofp == NULL || buffer == NULL) goto FAIL1;

   /* get filelen */
   if (fseek64(ofp, 0LL, SEEK_END) != 0) goto FAIL1;
   if ((filelen = ftell64(ofp)) == EOF) goto FAIL1;
   filecount = (size_t) (filelen / size);

   for (rewind(ofp); filecount > 0; filecount -= in) {
      /* read input file in chunks for presort */
      if (filecount < count) count = filecount;
      in = fread(buffer, size, count, ofp);
      if (in < count && ferror(ofp)) goto FAIL1;
      if (fseek(ofp, -(in * size), SEEK_CUR) != 0) goto FAIL1;
      /* check data was read */
      if (in > 0) {
         /* perform sort on buffer data, write to output */
         if (in > 1) qsort(buffer, in, size, comp);
         if (fwrite(buffer, size, in, ofp) != in) goto FAIL1;
      }
   }
   /* cleanup */
   fclose(ofp);
   free(buffer);

   /* PHASE 2: merge sort blocks together until nothing left to sort */

   /* obtain file size */
   filelen = EOF;
   ofp = fopen(filename, "rb");
   if (ofp == NULL) return (-1);
   if (fseek64(ofp, 0LL, SEEK_END) == 0) filelen = ftell64(ofp);
   fclose(ofp);
   /* check filesize */
   if (filelen == EOF) return (-1);

   /* create comparison buffers */
   a = malloc(size);
   b = malloc(size);
   if (a == NULL || b == NULL) goto FAIL2;

   snprintf(fname, FILENAME_MAX, "%s.sort", filename);

   /* iterate until (sorted) block size is greater than total filesize */
   for (block = (long long) blocksz; block < filelen; block <<= 1) {
      /* open files for merge sorting */
      afp = fopen(filename, "rb");
      bfp = fopen(filename, "rb");
      ofp = fopen(fname, "wb");
      if (afp == NULL || bfp == NULL || ofp == NULL) goto FAIL2_IO;
      /* iterate over every "block pair", shifting end to start */
      for (start = 0; start < filelen; start = end) {
         /* set index parameters */
         mid = start + block;
         end = mid + block;
         if (mid > filelen) mid = end = filelen;
         else if (end > filelen) end = filelen;
         aidx = start;
         bidx = mid;

         /* pre-read first values into buffers */
         if (fseek64(afp, aidx, SEEK_SET) != 0) goto FAIL2_IO;
         if (fread(a, size, 1, afp) != 1) goto FAIL2_IO;
         if (bidx < end) {
            if (fseek64(bfp, bidx, SEEK_SET) != 0) goto FAIL2_IO;
            if (fread(b, size, 1, bfp) != 1) goto FAIL2_IO;
         }
         /* walk the block pair until data is (merge) sorted */
         while (aidx < mid || bidx < end) {
            if (aidx >= mid) cond = 1;
            else if (bidx >= end) cond = -1;
            else cond = comp(a, b);
            /* determine comparison result */
            if (cond <= 0) {
               /* write a to output and read another (if available) */
               if (fwrite(a, size, 1, ofp) != 1) goto FAIL2_IO;
               aidx += size;
               if (aidx < mid) {
                  if (fread(a, size, 1, afp) != 1) goto FAIL2_IO;
               }
            } else {
               /* write b to output and read another (if available) */
               if (fwrite(b, size, 1, ofp) != 1) goto FAIL2_IO;
               bidx += size;
               if (bidx < end) {
                  if (fread(b, size, 1, bfp) != 1) goto FAIL2_IO;
               }
            }
         }
      }
      /* close files and move result back to filename */
      fclose(ofp);
      fclose(bfp);
      fclose(afp);
      if (remove(filename) != 0) goto FAIL2;
      if (rename(fname, filename) != 0) goto FAIL2;
   }

   /* sort success */
   return 0;

/* error handling */
FAIL_INVAL: set_errno(EINVAL); return (-1);
FAIL2_IO:
   if (ofp) fclose(ofp);
   if (bfp) fclose(bfp);
   if (afp) fclose(afp);
FAIL2:
   if (b) free(b);
   if (a) free(a);
   return (-1);
FAIL1:
   if (buffer) free(buffer);
   if (ofp) fclose(ofp);
   return (-1);
}  /* end filesort() */

/* end include guard */
#endif
