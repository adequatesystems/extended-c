
#include "_assert.h"
#define DISABLE_WORD64
#include "../extint.h"

int main()
{  /* check; returned carry, and final result */
   word32 numA[2] = { WORD32_C(0xc0c0aace), WORD32_C(0xc0ffee00) };
   word32 numB[2] = { WORD32_C(0xdeadbeef), WORD32_C(0xcafef00d) };

   ASSERT_EQ(cmp64(&numA, &numB), -1);
   ASSERT_EQ(cmp64(&numB, &numA), 1);
   ASSERT_EQ(cmp64(&numA, &numA), 0);
}
