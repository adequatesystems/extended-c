/**
 * extinet.c - Extended internet support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifndef EXTENDED_INTERNET_C
#define EXTENDED_INTERNET_C  /* include guard */


#include "extinet.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/**
 * Print local host info on stdout.
 * Returns 0 on succesful operation, or -1 on error. */
int phostinfo(void)
{
   int result, i;
   char hostname[100], *addrstr;
   struct hostent *host = NULL;

   /* get local machine name and IP address */
   result = gethostname(hostname, sizeof(hostname));
   if(result == 0) host = gethostbyname(hostname);
   printf("Local Machine Info\n");
   if (host) {
      printf("  Machine name: %s\n", host->h_name ? host->h_name : "unknown");
      for (i = 0; host->h_aliases[i]; i++) {
         printf("     alt. name: %s\n", host->h_aliases[i]);
      }
      for (i = 0; host->h_addr_list[i]; i++) {
         addrstr = inet_ntoa(*((struct in_addr *) (host->h_addr_list[i])));
         if (host->h_addrtype == AF_INET) {
            printf("  IPv4 address: %s\n", addrstr);
         } else printf("  Unknown address: %s\n", addrstr);
      }
   } else printf("  Machine name: unknown\n  IPv4 address: unknown\n");

   printf("\n");
   return 0;
}  /* end phostinfo() */

/**
 * Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
 * Place in char *a if provided, else use static char *cp.
 * Returns char* to converted result. */
char *ntoa(void *n, char *a)
{
   static char cp[16];
   unsigned char *bp;

   bp = (unsigned char *) n;
   if (a == NULL) a = cp;
   sprintf(a, "%d.%d.%d.%d", bp[0], bp[1], bp[2], bp[3]);

   return a;
}  /* end ntoa() */

/**
 * Perform an ip address lookup on the network address string *a.
 * Returns a network ip on success, or 0 on error.
 * NOTES:
 * - *a can be a hostname or numbers-and-dots notation IPv4. */
unsigned long aton(char *a)
{
   struct hostent *host;
   struct sockaddr_in addr;

   if(a == NULL) return 0;
   memset(&addr, 0, sizeof(addr));

   host = gethostbyname(a);
   if(host == NULL) return 0;
   memcpy(&(addr.sin_addr.s_addr), host->h_addr_list[0], host->h_length);

   return addr.sin_addr.s_addr;
}  /* end aton() */

/**
 * Obtain an ip address from a valid SOCKET descriptor.
 * Returns a network ip on success, or INVALID_SOCKET on error,
 * and a specific error code can be retrieved with get_sock_err(). */
unsigned long get_sock_ip(SOCKET sd)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;

   if(getpeername(sd, (struct sockaddr *) &addr, &addrlen) == 0) {
      return addr.sin_addr.s_addr;  /* the unsigned long ip */
   }

   return (unsigned long) SOCKET_ERROR;
}  /* end get_sock_ip() */

/**
 * Initialize an outgoing connection with ip on port.
 * Returns non-blocking SOCKET descriptor on success,
 * or INVALID_SOCKET on error. */
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;
   time_t start;
   SOCKET sd;
   int ecode;
printf("sock_connect_ip(): 1\n");
for (int i = 0; i < 1000; i++) sock_sleep();
   /* AF_INET = IPv4 */
   if((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
      return INVALID_SOCKET;
   }
printf("sock_connect_ip(): 2\n");
for (int i = 0; i < 1000; i++) sock_sleep();
   memset((char *) &addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = ip;
   addr.sin_family = AF_INET;  /* AF_UNIX */
   /* Convert short integer to network byte order */
   addr.sin_port = htons(port);
printf("sock_connect_ip(): 3\n");
for (int i = 0; i < 1000; i++) sock_sleep();
   sock_set_nonblock(sd);
   time(&start);
printf("sock_connect_ip(): 4\n");
for (int i = 0; i < 1000; i++) sock_sleep();
   while (connect(sd, (struct sockaddr *) &addr, addrlen)) {
      ecode = get_sock_err();
      if (sock_err_is_success(ecode)) return sd;
      if (sock_err_is_waiting(ecode)) {
         if (difftime(time(NULL), start) >= timeout) break;
         sock_sleep();  /* socket is waiting patiently */
      }
   }
printf("sock_connect_ip(): 5\n");
for (int i = 0; i < 1000; i++) sock_sleep();
   /* cleanup */
   sock_close(sd);
   return INVALID_SOCKET;
}  /* end sock_connect_ip() */

/**
 * Receive a packet of data from SOCKET sd into pkt[len].
 * The timeout is only effective for non-blocking sockets.
 * Return 0 for success, else 1 on error or (-1) on timeout */
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) recv(sd, ((char *) pkt) + n, len - n, flags);
      if (count > 0) n += count;  /* count recv'd bytes */
      else if (count < 0) {  /* waiting, check timeout */
         if(difftime(time(NULL), start) >= timeout) return (-1);
         sock_sleep();  /* socket is waiting patiently */
      } else return 1;  /* socket was closed gracefully */
   }

   return 0;  /* recv'd packet */
}  /* end sock_recv() */

/**
 * Send a packet of data on SOCKET sd from pkt[len].
 * The timeout is only effective for non-blocking sockets.
 * Return 0 for success, else 1 on error or (-1) on timeout */
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) send(sd, ((char *) pkt) + n, len - n, flags);
      if (count > 0) n += count;  /* count sent bytes */
      else if (count < 0) {  /* waiting, check timeout */
         if(difftime(time(NULL), start) >= timeout) return (-1);
         sock_sleep();  /* socket is waiting patiently */
      } else return 1;  /* socket was closed gracefully */
   }

   return 0;  /* sent packet */
}  /* end sock_send() */


#endif  /* end EXTENDED_INTERNET_C */
