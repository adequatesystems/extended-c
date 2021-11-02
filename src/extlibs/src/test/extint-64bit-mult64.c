
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word64 numA = WORD64_C(0x00000000c0c0aace);
   word64 numB = WORD64_C(0xcafef00ddeadbeef);
   word64 expect = WORD64_C(0x15f7b28b03da5a52);
   word64 result = 0;

   ASSERT_EQ(mult64(&result, &numB, &result), 0);
   ASSERT_EQ(mult64(&numB, &numB, &result), 1);
   ASSERT_EQ(mult64(&numB, &numA, &result), 1);
   ASSERT_EQ(mult64(&numA, &numA, &result), 0);
   ASSERT_EQ(mult64(&numA, &numB, &result), 1);
   ASSERT_EQ(result, expect);
}
