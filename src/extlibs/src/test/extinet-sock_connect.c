
#include "_assert.h"
#include "../extinet.h"

int main()
{  /* check; SockAbort initializes Zero, SockAbort fails connectip(),
    *        and connectip() connects to listening sockets */
   ASSERT_EQ_MSG(SockAbort, 0, "SockAbort should initialize Zero (0)");
   SockAbort = 1;
   ASSERT_EQ_MSG(sock_connect_addr("example.com", 80, 3), INVALID_SOCKET,
      "connection should fail when SockAbort is set");
   SockAbort = 0;
   ASSERT_EQ_MSG(sock_connect_addr("localhost", 443, 3), INVALID_SOCKET,
      "connection should timeout for connection to localhost:443");
   ASSERT_NE_MSG(sock_connect_addr("example.com", 80, 3), INVALID_SOCKET,
      "connection should succeed after listener is created");
}
