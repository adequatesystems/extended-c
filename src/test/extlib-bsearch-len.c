
#include "_assert.h"
#include "../extlib.h"

#include <stdio.h>

#define SORTSZ ( 32LL )
#define ITEMS  ( 1234567 )
#define FSIZE  ( (SORTSZ * ITEMS) ) /* ~38M */

int comp(const void *a, const void *b)
{
   return memcmp(a, b, SORTSZ);
}

int main()
{
   char *buf;
   size_t i, idx;
   char buf3[SORTSZ] = { 0 };

   /* write random data to file */
   ASSERT_NE((buf = malloc(FSIZE)), NULL);
   for (idx = 0; idx < FSIZE; idx += sizeof(int)) {
      *(int *) (buf + idx) = rand();
   }

   qsort(buf, ITEMS, SORTSZ, comp);

   ASSERT_EQ(bsearch_len(buf3, SORTSZ, buf, ITEMS, SORTSZ), NULL);

   /* find every item */
   for (i = SORTSZ; i < FSIZE; i += SORTSZ) {
      ASSERT_EQ(
         bsearch_len((buf + i), SORTSZ, buf, ITEMS, SORTSZ),
         (buf + i));
   }

   free(buf);
}
