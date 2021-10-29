
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check connectip() can connect to listening sockets */
   ASSERT_EQ_MSG(connectaddr("localhost", 443), INVALID_SOCKET,
      "connection should timeoutfor connection to localhost:443");
   ASSERT_NE_MSG(connectaddr("google.com", 443), INVALID_SOCKET,
      "connection should succeed after listener is created");
}
