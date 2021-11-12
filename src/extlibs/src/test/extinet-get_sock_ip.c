
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check getsocketip() returns appropriate value */
   SOCKET sd;

   ASSERT_NE_MSG((sd = connectaddr("google.com", 443)), INVALID_SOCKET,
      "connection failed, but needs to succeed to test getsocketip()");
   ASSERT_EQ(getsocketip(sd), aton("google.com"));
   ASSERT_EQ(getsocketip(INVALID_SOCKET), (unsigned long) INVALID_SOCKET);
}
