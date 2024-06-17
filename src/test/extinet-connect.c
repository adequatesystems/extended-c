
#include "_assert.h"
#include "../extinet.h"
#include "../exttime.h"

#include <stdio.h>

int main()
{
   struct sockaddr_in addr;
   struct sockaddr *addrp;
   time_t start;
   SOCKET sd;

   wsa_startup(2, 2);  /* enables socket support */

   /* prepare address with invalid endpoint */
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(12345);
   addr.sin_addr.s_addr = aton("example.com");
   addrp = (struct sockaddr *) &addr;

   /* INVALID connect_timed socket */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "socket creation failed");
   ASSERT_NE_MSG(connect_timed(sd, addrp, sizeof(addr), 1), 0,
      "connection should timeout on invalid endpoint");
   closesocket(sd);

   /* INVALID connect_auto to socket (non-blocking) */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "socket creation failed");
   ASSERT_EQ_MSG(set_nonblocking(sd), 0, "should set socket to nonblocking");
   for (time(&start); connect_auto(sd, addrp) != 0; millisleep(100)) {
      int ecode = socket_errno;
      ASSERT_NE(socket_is_connecting(ecode), 0);
      if (difftime(time(NULL), start) < 1) break;
   }
   closesocket(sd);

   /* update address with valid endpoint */
   addr.sin_port = htons(80);

   /* VALID connect_timed socket */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "socket creation failed");
   ASSERT_EQ_MSG(connect_timed(sd, addrp, sizeof(addr), 3), 0,
      "connection should succeed on valid endpoint");
   closesocket(sd);

   /* VALID connect_auto to socket (non-blocking) */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "socket creation failed");
   ASSERT_EQ_MSG(set_nonblocking(sd), 0, "should set socket to nonblocking");
   for (time(&start); connect_auto(sd, addrp) != 0; millisleep(100)) {
      ASSERT_NE(socket_is_connecting(socket_errno), 0);
      int ecode = socket_errno;
      if (socket_is_connected(ecode)) break;
      ASSERT_NE_MSG(socket_is_connecting(ecode), 0, "connection failed");
      ASSERT_LT_MSG(difftime(time(NULL), start), 3, "connection timeout");
   }
   closesocket(sd);

   wsa_cleanup();
}
