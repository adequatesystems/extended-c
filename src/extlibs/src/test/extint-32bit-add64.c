
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word32 numA[2] = { WORD32_C(0xc0c0aace) };
   word32 numB[2] = { WORD32_C(0xdeadbeef), WORD32_C(0xcafef00d) };
   word32 expect[2] = { WORD32_C(0x9f6e69bd), WORD32_C(0xcafef00e) };
   word32 result[2] = { 0 };

   ASSERT_EQ(add64_32(&result, &numB, &result), 0);
   ASSERT_EQ(add64_32(&numB, &numB, &result), 1);
   ASSERT_EQ(add64_32(&numB, &numA, &result), 0);
   ASSERT_EQ(add64_32(&numA, &numA, &result), 0);
   ASSERT_EQ(add64_32(&numA, &numB, &result), 0);
   ASSERT_CMP(result, expect, 8);
}
