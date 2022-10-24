/**
 * @private
 * @headerfile extinet.h <extinet.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_INTERNET_C
#define EXTENDED_INTERNET_C


#include "extinet.h"
#include "exttime.h"
#include <stdio.h>
#include <string.h>

#if OS_WINDOWS
   #pragma comment(lib, "winhttp.lib")
   #include <winhttp.h>
   #include <wchar.h>

   /* initialize global winsock version 2.2 */
   WSADATA Sockdata;
   WORD Sockverreq = 0x0202;

/* end Windows */
#elif OS_UNIX
   #include <fcntl.h>   /* for fcntl() */
   #include <stdlib.h>  /* for system() */

/* end Unix */
#endif

/* initialize global Sockinuse */
int Sockinuse;

/**
 * Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
 * Place in `char *a` if not `NULL`, else use static char *cp.
 * @param n Pointer to 32-bit value to convert
 * @param a Pointer to character array to place conversion result
 * @returns Character pointer to converted result.
*/
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
 * Convert a network ip address to a 32-bit binary form value by
 * performing an ip address lookup on a network address string.
 * Can use a hostname or numbers-and-dots notation IPv4.
 * @param a Pointer to network ip address character array to convert
 * @returns An IPv4 in 32-bit binary form on success, or 0 on error.
*/
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
 * @param sd Socket descriptor to obtain ip value from
 * @returns A network ip on success, or INVALID_SOCKET on error.
 * Obtain underlying error code with `sock_errno`.
*/
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
 * Perform Socket Startup enabling socket functionality.
 * @retval SOCKET_ERROR if Sockverreq was not met (Windows-only)
 * @retval sock_errno for other socket errors (Windows-only)
 * @retval 0 on success
*/
int sock_startup(void)
{
#if OS_WINDOWS
   /* perform startup request */
   int ecode = WSAStartup(Sockverreq, &Sockdata);
   if (ecode) return ecode;

#endif

   Sockinuse++;  /* increment to enable sockets support */

#if OS_WINDOWS
   /* ensure requested version was obtained */
   if (Sockdata.wVersion != Sockverreq) {
      return -1;
   }

#endif

   return 0;
}  /* end sock_startup() */

/**
 * Obtain the internal state of "sockets-in-use", as modified by functions
 * sock_startup() and sock_cleanup().
 * @returns Zero when sockets have not been started, else non-zero.
*/
int sock_state(void)
{
   return Sockinuse;
}

/**
 * Perform Socket Cleanup, disabling socket functionality.
 * @returns Always returns 0.
 * @note Associated socket support functions will gracefully shutdown.
*/
int sock_cleanup(void)
{
   while(Sockinuse) {

#if OS_WINDOWS
      WSACleanup();

#endif

      Sockinuse--;
   }

   return Sockinuse;
}  /* end sock_cleanup() */

/**
 * Set socket descriptor to non-blocking I/O.
 * @param sd Socket descriptor to set non-blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Obtain underlying error code with `sock_errno`.
*/
int sock_set_nonblock(SOCKET sd)
{
#if OS_WINDOWS
   u_long arg = 1L;
   return ioctlsocket(sd, FIONBIO, &arg);

#elif OS_UNIX
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

#endif
}  /* end sock_set_nonblock() */

/**
 * Set socket descriptor to blocking I/O.
 * @param sd Socket descriptor to set blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Obtain underlying error code with `sock_errno`.
*/
int sock_set_blocking(SOCKET sd)
{
#if OS_WINDOWS
   u_long arg = 0L;
   return ioctlsocket(sd, FIONBIO, &arg);

#elif OS_UNIX
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) & (~O_NONBLOCK));

#endif
}  /* end sock_set_blocking() */

/**
 * Close an open socket. Does NOT clear socket descriptor value.
 * @param sd socket descriptor to close
*/
int sock_close(SOCKET sd)
{
#if OS_WINDOWS
   return closesocket(sd);

#elif OS_UNIX
   return close(sd);

#endif
}  /* end sock_close() */

/**
 * Initialize an outgoing connection with a network IPv4 address.
 * @param ip 32-bit binary form IPv4 value
 * @param port 16-bit binary form port value
 * @param timeout time, in seconds, to wait for connection
 * @returns Non-blocking SOCKET on success, or INVALID_SOCKET on error.
 * @note sock_startup() MUST be called to enable socket support.
 * @see sock_connect_addr()
*/
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;
   time_t start;
   SOCKET sd;
   int ecode;

   if (!Sockinuse) return INVALID_SOCKET;

   sd = socket(AF_INET, SOCK_STREAM, 0);  /* AF_INET = IPv4 */
   if(sd == INVALID_SOCKET) return INVALID_SOCKET;

   memset((char *) &addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = ip;
   addr.sin_family = AF_INET;  /* AF_UNIX */
   /* Convert short integer to network byte order */
   addr.sin_port = htons(port);

   sock_set_nonblock(sd);
   time(&start);

   while (connect(sd, (struct sockaddr *) &addr, addrlen)) {
      ecode = sock_errno;
      if (sock_err_is_success(ecode)) break;
      if (sock_err_is_waiting(ecode) && Sockinuse &&
         (difftime(time(NULL), start) < timeout)) {
         millisleep(1);  /* socket is waiting patiently */
         continue;
      }  /* ... connection is deemed a failure, cleanup... */
      sock_close(sd);
      return INVALID_SOCKET;
   }

   return sd;
}  /* end sock_connect_ip() */

/**
 * Initialize an outgoing connection with a network host address.
 * @param addr Pointer to character array of network host address
 * @param port 16-bit binary form port value
 * @param timeout time, in seconds, to wait for connection
 * @returns Non-blocking SOCKET on success, or INVALID_SOCKET on error.
 * @note sock_startup() MUST be called to enable socket support.
 * @see sock_connect_ip()
*/
SOCKET sock_connect_addr(char *addr, unsigned short port, double timeout)
{
   unsigned long ip = aton(addr);

   return sock_connect_ip(ip, port, timeout);
}  /* end sock_connect_addr() */

/**
 * Receive a packet of data from a socket descriptor into `pkt[len]`.
 * Timeout is ignored if socket is set to blocking.
 * @param sd Socket descriptor to receive packet from
 * @param pkt Pointer to place received data
 * @param len Byte length of data to receive
 * @param flags Flags to pass to recv()
 * @param timeout time, in seconds, to wait for packet
 * @retval (0) for success
 * @retval (1) for end communication
 * @retval (-1) for timeout
*/
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) recv(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0 || !Sockinuse) return 1;  /* end communication */
      if (count > 0) n += count;  /* count recv'd bytes */
      else if(difftime(time(NULL), start) >= timeout) return (-1);
      else millisleep(1);  /* socket is waiting patiently */
   }

   return 0;  /* recv'd packet */
}  /* end sock_recv() */

/**
 * Send a packet of data on SOCKET sd from pkt[len].
 * Timeout is ignored if socket is set to blocking.
 * @param sd Socket descriptor to send packet on
 * @param pkt Pointer to data to send
 * @param len Byte length of data to send
 * @param flags Flags to pass to send()
 * @param timeout time, in seconds, to wait for send
 * @retval (0) for success
 * @retval (1) for end communication
 * @retval (-1) for timeout
*/
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) send(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0 || !Sockinuse) return 1;  /* end communication */
      if (count > 0) n += count;  /* count sent bytes */
      else if(difftime(time(NULL), start) >= timeout) return (-1);
      else millisleep(1);  /* socket is waiting patiently */
   }

   return 0;  /* sent packet */
}  /* end sock_send() */

/**
 * Get the IPv4 address of the host.
 * @returns 0 on succesful operation, else non-zero on error.
 * @note Requires sock_startup() to be previously called.
*/
int gethostip(char *name, int namelen)
{
   SOCKET sd;
   struct sockaddr_in addrname;
   socklen_t addrnamelen = sizeof(addrname);

   /* connect to cloudflare dns server 1.1.1.1:53 */
   sd = sock_connect_addr("1.1.1.1", 53, 3);
   if (sd < 0) return SOCKET_ERROR;

   /* get socket name for interpretation */
   if (getsockname(sd, (struct sockaddr *) &addrname, &addrnamelen) == 0) {
      if (inet_ntop(AF_INET, &addrname.sin_addr, name, namelen) != NULL) {
         return sock_close(sd);
      }
   }

   sock_close(sd);
   return SOCKET_ERROR;
}  /* end gethostip() */

/* end include guard */
#endif
