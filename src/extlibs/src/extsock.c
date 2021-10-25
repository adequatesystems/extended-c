/**
 * extsock.c - Extended socket support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 9 January 2018
 * Revised: 25 October 2021
 *
*/

#ifndef _EXTENDED_SOCKET_C_
#define _EXTENDED_SOCKET_C_  /* include guard */


#include "extsock.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifndef _WIN32  /* assume UNIXLIKE */

   #include <arpa/inet.h>
   #include <fcntl.h>
   #include <netdb.h>

   /* Set socket sd to non-blocking I/O.
    * Returns 0 on success, or SOCKET_ERROR on error, and
    * a specific error code can be retrieved with getsockerr(). */
   int nonblock(SOCKET sd)
   {
      int flags;

      flags = fcntl(sd, F_GETFL, 0);
      return fcntl(sd, F_SETFL, flags | O_NONBLOCK);
   }

   /* Set socket sd to blocking I/O.
    * Returns 0 on success, or SOCKET_ERROR on error, and
    * a specific error code can be retrieved with getsockerr(). */
   int blocking(SOCKET sd)
   {
      int flags;

      flags = fcntl(sd, F_GETFL, 0);
      return fcntl(sd, F_SETFL, flags & (~O_NONBLOCK));
   }
/* end UNIXLIKE */
#else  /* assume Windows */

   #include <ws2tcpip.h>  /* for socklen_t */

   /* Perform Winsock Startup. Returns 0 on success,
    * -1 if SockVerReq was not met, or other error code. */
   int sockstartup(void)
   {
      int ecode;

      /* perform startup request */
      ecode = WSAStartup(SockVerReq, &SockData);
      if (ecode == 0) SockStarted++;
      else return ecode;
      /* ensure requested version was obtained */
      if (SockData.wVersion != SockVerReq) {
         return -1;
      }

      return 0;
   }  /* end sockstartup() */

   /* Perform Winsock Cleanup for every call to sockstartup(). */
   void sockcleanup(void)
   {
      while (SockStarted--) WSACleanup();
   }

   /* Set socket sd to non-blocking I/O.
    * Returns 0 on success, or SOCKET_ERROR on error, and
    * a specific error code can be retrieved with getsockerr(). */
   int nonblock(SOCKET sd)
   {
      u_long arg = 1L;

      return ioctlsocket(sd, FIONBIO, &arg);
   }

   /* Set socket sd to non-blocking I/O.
    * Returns 0 on success, or SOCKET_ERROR on error, and
    * a specific error code can be retrieved with getsockerr(). */
   int blocking(SOCKET sd)
   {
      u_long arg = 0L;

      return ioctlsocket(sd, FIONBIO, &arg);
   }

#endif  /* end Windows */

/* Set socket sd to non-blocking I/O.
 * Returns a network ip on success, or INVALID_SOCKET on error,
 * and a specific error code can be retrieved with getsockerr(). */
unsigned long getsocketip(SOCKET sd)
{
   struct sockaddr_in addr;
   socklen_t addrlen;

   addrlen = (socklen_t) sizeof(addr);
   if(getpeername(sd, (struct sockaddr *) &addr, &addrlen) == 0) {
      return addr.sin_addr.s_addr;  /* the unsigned long ip */
   }
   return (unsigned long) INVALID_SOCKET;
}  /* end getsocketip() */

/* Initialize an outgoing connection with ip on port.
 * Returns descriptor identifying a SOCKET on success,
 * or INVALID_SOCKET on error. */
SOCKET connectip(unsigned long ip, unsigned short port)
{
   static size_t sizeof_addr = sizeof(struct sockaddr);

   SOCKET sd;
   struct sockaddr_in addr;
   time_t timeout;
   int ecode;

   /* AF_INET = IPv4 */
   if((sd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
      return INVALID_SOCKET;
   }

   memset((char *) &addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = ip;
   addr.sin_family = AF_INET;  /* AF_UNIX */
   /* Convert short integer to network byte order */
   addr.sin_port = htons(port);

   nonblock(sd);  /* was after connect() v.21 */
   timeout = time(NULL) + 3;  /* 3 second connection timeout */

   while (!SockAbort && connect(sd, (struct sockaddr *) &addr, sizeof_addr)) {
      ecode = getsockerr();
      if (connect_success(ecode)) break;
      if (connect_waiting(ecode) && time(NULL) < timeout ) {
         /* RECOMMEND: microsleep wouldn't hurt here */
         continue;
      } /* ... connection is deemed a failure, cleanup... */
      closesocket(sd);
      return INVALID_SOCKET;
   }
   return sd;
}  /* end connectip() */

/* Print local host info on stdout.
 * Returns 0 on succesful operation, or -1 on error. */
int phostinfo(void)
{
   int result, i;
   char hostname[100], *addrstr;
   struct hostent *host;

   /* get local machine name and IP address */
   result = gethostname(hostname, sizeof(hostname));
   if(result == SOCKET_ERROR) {
     return -1;
   }
   host = gethostbyname(hostname);
   if(host == NULL) {
      return -1;
   }
   printf("Local Machine Info\n");
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

    return 0;
}  /* end phostinfo() */

/* Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
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

/* Perform an ip address lookup on the network address string *a.
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


#endif  /* end _EXTENDED_SOCKET_C_ */
