
#include <stdlib.h>
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check connectip() can connect to listening sockets */
   ASSERT_EQ_MSG(connectaddr("localhost", 54321), INVALID_SOCKET,
      "connection should timeout, before listener is created");
   system("nc -l 54321 &");  /* creates listener in background */
   ASSERT_NE_MSG(connectaddr("localhost", 54321), INVALID_SOCKET,
      "connection should succeed after listener is created");
}
