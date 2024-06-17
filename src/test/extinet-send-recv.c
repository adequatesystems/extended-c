
#include "_assert.h"
#include "../extinet.h"

int main()
{
   char *request = "GET / HTTP/1.1\r\nHost: example.com\r\n\r\n";
   char response[4096] = "";
   struct sockaddr_in addr;
   struct sockaddr *addrp;
   SOCKET sd;

   wsa_startup(2, 2);  /* enables socket support */

   /* prepare address with invalid endpoint */
   memset(&addr, 0, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(80);
   addr.sin_addr.s_addr = aton("example.com");
   addrp = (struct sockaddr *) &addr;

   /* create socket */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   ASSERT_NE_MSG(sd, INVALID_SOCKET, "socket creation failed");
   ASSERT_EQ_MSG(connect_timed(sd, addrp, sizeof(addr), 3), 0,
      "connection should succeed on valid endpoint");
   ASSERT_EQ_MSG(send_timed(sd, request, (int) strlen(request), 0, 3), 0,
      "sock_send() should successfully send request packet");
   ASSERT_EQ_MSG(recv_timed(sd, response, 1, 0, 3), 0,
      "sock_recv() should recv a single byte as a packet");
   ASSERT_EQ_MSG(recv_timed(sd, &response[1], 4095, 0, 1), (-1),
      "sock_recv() should timeout after receiving entire response");
   ASSERT_GT_MSG(strlen(response), 0, "response should be non-zero");
   closesocket(sd);
   ASSERT_EQ_MSG(send_timed(sd, request, (int) strlen(request), 0, 1), (-1),
      "sock_send() should timeout, socket is closed");

   wsa_cleanup();
}
