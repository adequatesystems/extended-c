
#include "../extassert.h"
#include "../extlib.h"

int main()
{  /* check put16() places 16-bit unsigned value in array */
   unsigned char array[8] = { 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };
   unsigned char expect[8] = { 0xff, 0x7f, 0x3f, 0x1f, 0xf, 0x7, 0x3, 0x1 };
   put16(array, WORD16_C(0x7fff)); ASSERT_CMP(array, expect, 2);
}
