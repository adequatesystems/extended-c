
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; final result */
   word64 numA = WORD64_C(0xc0ffee00c0c0aace);
   word64 expect = WORD64_C(0x607ff70060605567);

   shiftr64(&numA);
   ASSERT_EQ(numA, expect);
}
