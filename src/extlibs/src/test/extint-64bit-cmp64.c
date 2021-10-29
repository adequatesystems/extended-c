
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word64 numA = WORD64_C(0xc0ffee00c0c0aace);
   word64 numB = WORD64_C(0xcafef00ddeadbeef);

   ASSERT_EQ(cmp64(&numA, &numB), -1);
   ASSERT_EQ(cmp64(&numB, &numA), 1);
   ASSERT_EQ(cmp64(&numA, &numA), 0);
}