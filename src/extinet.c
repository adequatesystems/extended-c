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

/* internal support */
#include "exterrno.h"
#include "exttime.h"

/* external support */
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
   /* initialize for winsock version 2.2 */
   WSADATA Sockdata;
   WORD Sockverreq = 0x0202;

/* end Windows */
#else
   #include <fcntl.h>   /* for fcntl() */

/* end UNIX-like */
#endif

/* initialize global Sockinuse */
int Sockinuse;

/**
 * Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
 * Converted result is placed in @a *a if supplied, otherwise an internal
 * static character pointer is used.
 * @param n Pointer to 32-bit value to convert
 * @param a Pointer to character array to place conversion result
 * @returns Character pointer to converted result, or NULL on error.
 * Check errno for details.
*/
char *ntoa(void *n, char *a)
{
   static char cp[16];
   unsigned char *bp;

   if (n == NULL) goto FAIL_INVAL;
   if (a == NULL) a = cp;

   bp = (unsigned char *) n;
   sprintf(a, "%d.%d.%d.%d", bp[0], bp[1], bp[2], bp[3]);

   return a;

/* error handling */
FAIL_INVAL: set_errno(EINVAL); return NULL;
}  /* end ntoa() */

/**
 * Convert a network ip address to binary form. Performs network address
 * lookup. Can use a hostname or numbers-and-dots notation IPv4.
 * @param a Pointer to network ip address character array to convert
 * @returns Unsigned long value of the associated network IPv4 address
 * in binary form, or 0 on error. Check errno for details.
*/
unsigned long aton(char *a)
{
   struct hostent *host;
   struct sockaddr_in addr;

   if (a == NULL) goto FAIL_INVAL;
   memset(&addr, 0, sizeof(addr));

   host = gethostbyname(a);
   if (host == NULL) goto FAIL_HOST;
   memcpy(&(addr.sin_addr.s_addr), host->h_addr_list[0], host->h_length);

   return addr.sin_addr.s_addr;

/* error handling */
FAIL_INVAL: set_errno(EINVAL); return 0;
FAIL_HOST:
#ifdef _WIN32
   set_alterrno(WSAGetLastError());
#endif
   return 0;
}  /* end aton() */

/**
 * Obtain an ip address from a valid SOCKET descriptor.
 * @param sd Socket descriptor to obtain ip value from
 * @returns Unsigned long value of the IPv4 address in binary form, or
 * SOCKET_ERROR on error. Check errno for details.
*/
unsigned long get_sock_ip(SOCKET sd)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;

   if (getpeername(sd, (struct sockaddr *) &addr, &addrlen) == 0) {
      return addr.sin_addr.s_addr;  /* the unsigned long ip */
   }

#ifdef _WIN32
   set_alterrno(WSAGetLastError());
#endif

   return (unsigned long) SOCKET_ERROR;
}  /* end get_sock_ip() */

/**
 * Perform Socket Startup enabling socket functionality.
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int sock_startup(void)
{
#ifdef _WIN32
   /* perform startup request -- ensure requested version was obtained */
   int ecode = WSAStartup(Sockverreq, &Sockdata);
   if (ecode) {
      set_alterrno(ecode);
      return SOCKET_ERROR;
   }

#endif

   Sockinuse++;  /* increment to enable sockets support */

   return 0;
}  /* end sock_startup() */

/**
 * Obtain the internal state of "sockets-in-use", as modified by functions
 * sock_startup() and sock_cleanup().
 * @returns Value representing the number of sock_startup() calls made.
*/
int sock_state(void)
{
   return Sockinuse;
}

/**
 * Perform Socket Cleanup, disabling socket functionality.
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int sock_cleanup(void)
{
   int ecode = 0;

   while (Sockinuse > 0) {

#ifdef _WIN32
   if (WSACleanup()) {
      set_alterrno(WSAGetLastError());
      return SOCKET_ERROR;
   }

#endif

      Sockinuse--;
   }

   return ecode;
}  /* end sock_cleanup() */

/**
 * Set socket descriptor to non-blocking I/O.
 * @param sd Socket descriptor to set non-blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int sock_set_nonblock(SOCKET sd)
{
#ifdef _WIN32
   u_long arg = 1UL;
   int ecode = ioctlsocket(sd, FIONBIO, &arg);
   if (ecode) set_alterrno(WSAGetLastError());
   return ecode;

#else
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);

#endif
}  /* end sock_set_nonblock() */

/**
 * Set socket descriptor to blocking I/O.
 * @param sd Socket descriptor to set blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int sock_set_blocking(SOCKET sd)
{
#ifdef _WIN32
   u_long arg = 0UL;
   int ecode = ioctlsocket(sd, FIONBIO, &arg);
   if (ecode) set_alterrno(WSAGetLastError());
   return ecode;

#else
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) & (~O_NONBLOCK));

#endif
}  /* end sock_set_blocking() */

/**
 * Close an open socket. Does NOT clear socket descriptor value.
 * @param sd socket descriptor to close
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int sock_close(SOCKET sd)
{
#ifdef _WIN32
   int ecode = closesocket(sd);
   if (ecode) set_alterrno(WSAGetLastError());
   return ecode;

#else
   return close(sd);

#endif
}  /* end sock_close() */

/**
 * Initialize an outgoing connection with a network IPv4 address.
 * @param ip 32-bit binary form IPv4 value
 * @param port 16-bit binary form port value
 * @param timeout time, in seconds, to wait for connection
 * @returns Non-blocking SOCKET on success, or INVALID_SOCKET on error.
 * Check errno for details.
*/
SOCKET sock_connect_ip
   (unsigned long ip, unsigned short port, double timeout)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;
   time_t start;
   SOCKET sd;
   int ecode;

   sd = socket(AF_INET, SOCK_STREAM, 0);  /* AF_INET = IPv4 */
   if(sd == INVALID_SOCKET) {
      ecode = sock_errno;
      goto FAIL;
   }

   memset((char *) &addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = ip;
   addr.sin_family = AF_INET;  /* AF_UNIX */
   /* Convert short integer to network byte order */
   addr.sin_port = htons(port);

   sock_set_nonblock(sd);
   time(&start);

   while (connect(sd, (struct sockaddr *) &addr, addrlen)) {
      ecode = sock_errno;
      if (sock_connected(ecode)) break;
      if (sock_connecting(ecode) &&
         (difftime(time(NULL), start) < timeout)) {
         millisleep(1);  /* socket is waiting patiently */
         continue;
      }  /* ... connection is deemed a failure, cleanup... */
      sock_close(sd);
      goto FAIL;
   }

   return sd;

/* error handling */
FAIL:
#ifdef _WIN32
   set_alterrno(ecode);
#else
   set_errno(ecode);
#endif
   return INVALID_SOCKET;
}  /* end sock_connect_ip() */

/**
 * Initialize an outgoing connection with a network host address.
 * @param addr Pointer to character array of network host address
 * @param port 16-bit binary form port value
 * @param timeout time, in seconds, to wait for connection
 * @returns Non-blocking SOCKET on success, or INVALID_SOCKET on error.
 * Check errno for details.
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
 * @returns (int) value representing the operation result
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
      if (count == 0) return 1;  /* end communication */
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
 * @returns (int) value representing the operation result
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
      if (count == 0) return 1;  /* end communication */
      if (count > 0) n += count;  /* count sent bytes */
      else if(difftime(time(NULL), start) >= timeout) return (-1);
      else millisleep(1);  /* socket is waiting patiently */
   }

   return 0;  /* sent packet */
}  /* end sock_send() */

/**
 * Get the IPv4 address of the host.
 * @returns 0 on succesful operation, or SOCKET_ERROR on error.
 * Check errno for details.
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

#ifdef _WIN32
   set_alterrno(WSAGetLastError());
#endif

   sock_close(sd);
   return SOCKET_ERROR;
}  /* end gethostip() */

/* end include guard */
#endif
