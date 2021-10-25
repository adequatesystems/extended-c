
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check aton() converts domain to 32-bit unsigned long */
   ASSERT_EQ(aton("localhost"), 0x100007fUL);
}
