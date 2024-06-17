/**
 * @private
 * @headerfile extinet.h <extinet.h>
 * @copyright Adequate Systems LLC, 2018-2024. All Rights Reserved.
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

#ifndef _WIN32
   /* system support */
   #include <fcntl.h>   /* for fcntl() */

/* end UNIX-like */
#endif

/* always online reference addresses */
#define CLOUDFLARE_DNS_IPv4 "1.1.1.1"
#define CLOUDFLARE_DNS_IPv6 "2606:4700:4700::1111"
#define CLOUDFLARE_DNS_PORT 53

/**
 * Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
 * Converted result is placed in @a *a if supplied, otherwise an internal
 * static character pointer is used.
 * @param n Pointer to 32-bit value to convert
 * @param a Pointer to character array to place conversion result
 * @returns Character pointer to converted result, or NULL on error.
 * Check errno for details.
*/
char *ntoa(void *n, char a[16])
{
   static char cp[16];
   unsigned char *bp;

   if (n == NULL) {
      set_errno(EINVAL);
      return NULL;
   }

   if (a == NULL) a = cp;
   bp = (unsigned char *) n;
   snprintf(a, 16, "%d.%d.%d.%d", bp[0], bp[1], bp[2], bp[3]);

   return a;
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

   if (a == NULL) {
      set_errno(EINVAL);
      return 0;
   }

   host = gethostbyname(a);
   if (host == NULL) {
#ifdef _WIN32
      set_alterrno(WSAGetLastError());
#endif
      return 0;
   }

   memset(&addr, 0, sizeof(addr));
   memcpy(&(addr.sin_addr.s_addr), host->h_addr_list[0], host->h_length);

   return addr.sin_addr.s_addr;
}  /* end aton() */

/**
 * Connect to a provided address and socket descriptor. This function
 * automatically determines the address family and connects to the
 * socket using the socklen_t parameter derived from the address family.
 * @param sd Socket descriptor to connect
 * @param addrp Socket address struct to connect to
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
*/
int connect_auto(SOCKET sd, const struct sockaddr *addrp)
{
   switch (addrp->sa_family) {
   #ifndef _WIN32
      case AF_UNIX: return connect(sd, addrp, sizeof(struct sockaddr_un));
   #endif
      case AF_INET: return connect(sd, addrp, sizeof(struct sockaddr_in));
      case AF_INET6: return connect(sd, addrp, sizeof(struct sockaddr_in6));
      default: set_errno(EAFNOSUPPORT); return SOCKET_ERROR;
   }
}  /* end connect_auto() */

/**
 * Connect to a provided address and socket descriptor. This function
 * will attempt to connect to the provided address and port, with a
 * specified timeout. If the connection is successful, the function
 * will return a non-blocking socket descriptor.
 * @param sd Socket descriptor to connect
 * @param addrp Socket address struct to connect to
 * @param len Length of the socket address struct
 * @param seconds Timeout in seconds
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Check socket_errno for details.
*/
int connect_timed
   (SOCKET sd, const struct sockaddr *addrp, socklen_t len, int seconds)
{
   time_t start;
   int ecode;

   /* set non-blocking */
   if (set_nonblocking(sd) != 0) return SOCKET_ERROR;
   /* try connect -- timeout after specified seconds */
   for (time(&start); connect(sd, addrp, len) != 0; millisleep(100)) {
      ecode = socket_errno;
      if (socket_is_connected(ecode)) break;
      if (socket_is_connecting(ecode)) {
         if (difftime(time(NULL), start) < (double) seconds) continue;
      }  /* ... connection is deemed a failure */
      return SOCKET_ERROR;
   }

   return 0;
}  /* end connect_timed() */

/**
 * Get the primary IPv4 address of the host machine.
 * @returns 0 on succesful operation, or SOCKET_ERROR on error.
 * Check socket_errno for details.
*/
int get_hostipv4(char *name, size_t namelen)
{
   static socklen_t len = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;
   struct sockaddr *addrp;
   SOCKET sd;
   int ecode;

   /* open socket for IPv4 connection */
   sd = socket(AF_INET, SOCK_STREAM, 0);
   if(sd == INVALID_SOCKET) return SOCKET_ERROR;

   /* set target to cloudflare dns */
   addrp = (struct sockaddr *) &addr;
   memset(addrp, 0, sizeof(struct sockaddr_in));
   addr.sin_family = AF_INET;
   addr.sin_port = htons(CLOUDFLARE_DNS_PORT);
   ecode = inet_pton(AF_INET, CLOUDFLARE_DNS_IPv4, &(addr.sin_addr));
   if (ecode != 1) {
      /* inet_pton return zero on invalid address string */
      if (ecode == 0) set_errno(EINVAL);
      goto FAIL;
   }

   /* connect (1s timeout), get socket name and convert */
   if (connect_timed(sd, addrp, len, 1) != 0) goto FAIL;
   if (getsockname(sd, addrp, &len) == 0) {
      if (inet_ntop(AF_INET, &(addr.sin_addr), name, namelen) != NULL) {
         return closesocket(sd);
      }
   }

/* error handling */
FAIL:
   closesocket(sd);
   return SOCKET_ERROR;
}  /* end get_hostipv4() */

/**
 * Get the primary IPv6 address of the host machine.
 * @returns 0 on succesful operation, or SOCKET_ERROR on error.
 * Check socket_errno for details.
*/
int get_hostipv6(char *name, size_t namelen)
{
   static socklen_t len = (socklen_t) sizeof(struct sockaddr_in6);
   struct sockaddr_in6 addr6;
   struct sockaddr *addrp;
   SOCKET sd;
   int ecode;

   /* open socket for IPv6 connection */
   sd = socket(AF_INET6, SOCK_STREAM, 0);
   if(sd == INVALID_SOCKET) return SOCKET_ERROR;

   /* set target to cloudflare dns */
   addrp = (struct sockaddr *) &addr6;
   memset(addrp, 0, len);
   addr6.sin6_family = AF_INET6;
   addr6.sin6_port = htons(CLOUDFLARE_DNS_PORT);
   addr6.sin6_scope_id = 0;
   ecode = inet_pton(AF_INET6, CLOUDFLARE_DNS_IPv6, &(addr6.sin6_addr));
   if (ecode != 1) {
      /* inet_pton return zero on invalid address string */
      if (ecode == 0) set_errno(EINVAL);
      goto FAIL;
   }

   /* connect (1s timeout), get socket name and convert */
   if (connect_timed(sd, addrp, len, 1) != 0) goto FAIL;
   if (getsockname(sd, addrp, &len) == 0) {
      if (inet_ntop(AF_INET6, &(addr6.sin6_addr), name, namelen) != NULL) {
         return closesocket(sd);
      }
   }

/* error handling */
FAIL:
   closesocket(sd);
   return SOCKET_ERROR;
}  /* end get_hostipv6() */

/**
 * Convert a sockaddr struct to a human-readable IP address.
 * @param addrp Pointer to a sockaddr struct
 * @param dst Destination buffer for IP address
 * @param size Size of the destination buffer
 * @returns Pointer to the destination buffer on success, or NULL on error.
 * Check socket_errno for details.
*/
const char *inet_ntop_auto
   (const struct sockaddr *src, char *dst, size_t size)
{
   /* determine family -- ipv4/6 only */
   switch (src->sa_family) {
      case AF_INET6:
         return inet_ntop(src->sa_family,
            &(((struct sockaddr_in6 *) src)->sin6_addr), dst, size);
      case AF_INET:
         return inet_ntop(src->sa_family,
            &(((struct sockaddr_in *) src)->sin_addr), dst, size);
      default: set_errno(EAFNOSUPPORT); return NULL;
   }
}

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
int recv_timed(SOCKET sd, void *pkt, int len, int flags, int seconds)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) recv(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0) return 1;  /* end communication */
      if (count > 0) n += count;  /* count recv'd bytes */
      else if(difftime(time(NULL), start) >= (double) seconds) return (-1);
      else millisleep(1);  /* socket is waiting patiently */
   }

   return 0;  /* recv'd packet */
}  /* end recv_timed() */

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
int send_timed(SOCKET sd, void *pkt, int len, int flags, int seconds)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) send(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0) return 1;  /* end communication */
      if (count > 0) n += count;  /* count sent bytes */
      else if(difftime(time(NULL), start) >= (double) seconds) return (-1);
      else millisleep(1);  /* socket is waiting patiently */
   }

   return 0;  /* sent packet */
}  /* end send_timed() */

/**
 * Set socket (file descriptor) to blocking I/O.
 * @param sd Socket to set blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Check sock_errno for details.
*/
int set_blocking(SOCKET sd)
{
#ifdef _WIN32
   return ioctlsocket(sd, FIONBIO, (u_long[1]) { 0UL });
#else
   int flags = fcntl(sd, F_GETFL, 0);
   if (flags == (-1)) return SOCKET_ERROR;
   return fcntl(sd, F_SETFL, flags & (~O_NONBLOCK));
#endif
}  /* end set_blocking() */

/**
 * Set socket (file descriptor) to non-blocking I/O.
 * @param sd Socket to set non-blocking I/O
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Check sock_errno for details.
*/
int set_nonblocking(SOCKET sd)
{
#ifdef _WIN32
   return ioctlsocket(sd, FIONBIO, (u_long[1]) { 1UL });
#else
   int flags = fcntl(sd, F_GETFL, 0);
   if (flags == (-1)) return SOCKET_ERROR;
   return fcntl(sd, F_SETFL, flags | O_NONBLOCK);
#endif
}  /* end set_nonblocking() */

/**
 * Cleanup and deregister the winsock dll from the implementation.
 * On non-Windows systems, this function does nothing.
 * @returns 0 on success, or SOCKET_ERROR on error.
 * Check socket_errno for details.
*/
int wsa_cleanup(void)
{
#ifdef _WIN32
   return WSACleanup();
#else
   return 0;
#endif
}

/**
 * Initialize the winsock dll for use with socket functions.
 * On non-Windows systems, this function does nothing.
 * @param major Major version of winsock dll to request
 * @param minor Minor version of winsock dll to request
 * @returns 0 on success, or SOCKET_ERROR on error. Check errno for details.
 * @exception ENOTSUP if requested version is not supported
*/
int wsa_startup(unsigned char major, unsigned char minor)
{
#ifdef _WIN32
   /* initialize for winsock dll request */
   WSADATA wsa_data;
   WORD wsa_request;
   int ecode;

   /* request winsock dll version (v<major>.<minor>) */
   wsa_request = MAKEWORD(major, minor);
   ecode = WSAStartup(wsa_request, &wsa_data);
   if (ecode != 0) {
      set_alterrno(ecode);
      return SOCKET_ERROR;
   }

   /* check supplied version */
   if (wsa_data.wVersion < wsa_request) {
      WSACleanup();
      set_errno(ENOTSUP);
      return SOCKET_ERROR;
   }

   return 0;
#else
   /* no compiler complaints */
   (void) major;
   (void) minor;

   return 0;
#endif
}  /* end wsa_startup() */

/* end include guard */
#endif
