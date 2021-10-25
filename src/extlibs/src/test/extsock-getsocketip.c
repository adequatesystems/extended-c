
#include <stdlib.h>
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check getsocketip() returns appropriate value */
   SOCKET sd;

   system("nc -l 54321 &");  /* so connection succeeds works */
   ASSERT_NE_MSG((sd = connectaddr("localhost", 54321)), INVALID_SOCKET,
      "connection failed, but needs to succeed to test getsocketip()");
   ASSERT_EQ(getsocketip(sd), 0x100007fUL);
   ASSERT_EQ(getsocketip(INVALID_SOCKET), (unsigned long) INVALID_SOCKET);
}
