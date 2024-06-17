
#include "_assert.h"
#include "../extinet.h"

int main()
{  /* check setting socket to blocking and non-blocking I/O */
   SOCKET sd;

   wsa_startup(2, 2);  /* enables socket support */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "connection is required for testing");
   ASSERT_EQ_MSG(set_blocking(sd), 0, "should set socket to blocking");
   ASSERT_EQ_MSG(set_nonblocking(sd), 0, "should set socket to nonblocking");
   wsa_cleanup();
}
