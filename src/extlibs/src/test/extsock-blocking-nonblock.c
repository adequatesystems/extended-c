
#include "_assert.h"
#include "../extsock.h"

int main()
{  /* check connectip() can connect to listening sockets */
   SOCKET sd;

   ASSERT_NE_MSG((sd = connectaddr("google.com", 443)), INVALID_SOCKET,
      "connection failed, but needs to succeed to test [non]blocking()");
   ASSERT_EQ_MSG(blocking(sd), 0, "should set socket to blocking");
   ASSERT_EQ_MSG(nonblock(sd), 0, "should set socket to nonblocking");
}
