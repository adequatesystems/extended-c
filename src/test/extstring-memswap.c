
#include "_assert.h"
#include "../extstring.h"

#include <stdio.h>

#define LISTLENGTH      ( BUFSIZ + 1 )
#define LISTHALFLENGTH  ( LISTLENGTH / 2 )

int main()
{  /* check; shuffle on list length < 2, and normal list */
   int expect_short[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
   int expect_long[LISTLENGTH] = { 0 };
   int list[LISTLENGTH] = { 0 };
   int i;

   /* initialize lists */
   for(i = 0; i < LISTLENGTH; i++) {
      expect_long[i] = i;
      list[i] = i;
   }
   for (i = 0; i < LISTHALFLENGTH; i++) {
      expect_long[i] = i + LISTHALFLENGTH;
      expect_long[i+ LISTHALFLENGTH] = i;
   }

   /* memswap large memory */
   memswap(list, &list[LISTHALFLENGTH], sizeof(list[0]) * LISTHALFLENGTH);
   ASSERT_CMP(list, expect_long, sizeof(expect_long));
   /* memswap small memory */
   memswap(list, &list[LISTHALFLENGTH], sizeof(list[0]) * 8);
   ASSERT_CMP(list, expect_short, sizeof(expect_short));
}
