
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check; random numbers do not exceed WORD16_MAX, and final seed */
   word32 iterations = WORD32_C(0x12345678);
   word32 expect = WORD32_C(0xadafae9f);
   word32 seedA, seedB, seedC;

   srand16(iterations, 0, 0);
   while (iterations--) ASSERT_LE(rand16(), WORD16_MAX);
   get_rand16(&seedA, &seedB, &seedC);
   ASSERT_EQ(seedA + seedB + seedC, expect);
}
