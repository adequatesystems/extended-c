
#include "_assert.h"
#include "../extinet.h"

int main()
{  /* check get_sock_ip() returns appropriate value */
   SOCKET sd;
   unsigned long ip;
printf("before aton\n");
   ASSERT_NE((ip = aton("google.com")), 0);
printf("before connect\n");
   ASSERT_NE((sd = sock_connect_ip(ip, 80, 3)), 0);
printf("after connect\n");

   ASSERT_NE_MSG(sd, INVALID_SOCKET, "connection is required for testing");
   ASSERT_EQ(get_sock_ip(sd), aton("google.com"));
   ASSERT_EQ(get_sock_ip(INVALID_SOCKET), (unsigned long) SOCKET_ERROR);
}
