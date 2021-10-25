
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; final result */
   word64 numA = WORD64_C(0xc0ffee00c0c0aace);
   word64 expect = WORD64_C(0x3f0011ff3f3f5532);

   negate64(&numA);
   ASSERT_EQ(numA, expect);
}
