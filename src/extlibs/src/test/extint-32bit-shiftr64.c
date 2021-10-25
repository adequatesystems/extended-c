
#include "_assert.h"
#define DISABLE_WORD64
#include "../extint.h"

int main()
{  /* check; final result */
   word32 numA[2] = { WORD32_C(0xc0c0aace), WORD32_C(0xc0ffee00) };
   word32 expect[2] = { WORD32_C(0x60605567), WORD32_C(0x607ff700) };

   shiftr64(&numA);
   ASSERT_CMP(numA, expect, 8);
}
