
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; random numbers do not exceed WORD16_MAX, and final seed */
   word32 iterations = WORD32_C(0x12345678);
   word32 expect = WORD32_C(0x73d207a0);

   srand16fast(iterations);
   while (iterations--) ASSERT_LE(rand16fast(), WORD16_MAX);
   ASSERT_EQ(get_rand16fast(), expect);
}
