
#include "_assert.h"
#define DISABLE_WORD64
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word32 numA[2] = { WORD32_C(0xc0c0aace), WORD32_C(0xc0ffee00) };
   word32 numB[2] = { WORD32_C(0xdeadbeef), WORD32_C(0xcafef00d) };
   word32 expect[2] = { WORD32_C(0x03da5a52), WORD32_C(0x0d8ae48b) };
   word32 result[2] = { 0 };

   ASSERT_EQ(mult64(&result, &numB, &result), 0);
   ASSERT_EQ(mult64(&numA, &numB, &result), 1);
   ASSERT_CMP(result, expect, 8);
}
