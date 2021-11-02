
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word64 numA = WORD64_C(0x00000000c0c0aace);
   word64 numB = WORD64_C(0xcafef00ddeadbeef);
   word64 expect = WORD64_C(0xcafef00e9f6e69bd);
   word64 result = 0;

   ASSERT_EQ(add64(&result, &numB, &result), 0);
   ASSERT_EQ(add64(&numB, &numB, &result), 1);
   ASSERT_EQ(add64(&numB, &numA, &result), 0);
   ASSERT_EQ(add64(&numA, &numA, &result), 0);
   ASSERT_EQ(add64(&numA, &numB, &result), 0);
   ASSERT_EQ(result, expect);
}