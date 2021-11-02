
#include "_assert.h"
#include "../extint.h"

int main()
{  /* check iszero() appropriately returns falsey and truthy values */
   unsigned char nonzero[9] = { 0, 0x1, 0x3, 0x7, 0xf, 0x1f, 0x3f, 0x7f, 0xff };
   unsigned char zero[9] = { 0 };
   ASSERT_EQ(iszero_32(nonzero, 9), 0);
   ASSERT_EQ(iszero_32(zero, 9), 1);
}
