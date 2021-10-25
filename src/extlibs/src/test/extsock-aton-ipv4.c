
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check aton() converts numbers-and-dots notation to 32-bit unsigned long */
   ASSERT_EQ(aton("255.207.159.111"), 0x6f9fcfffUL);
}
