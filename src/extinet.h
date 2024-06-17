/**
 * @file extinet.h
 * @brief Extended internet support.
 * @details Provides extended support for internet related functionality.
 * Applications targetting Windows should call wsa_startup() before using
 * any socket related functions (provided by "extinet" or otherwise) and
 * call wsa_cleanup() when no more socket related functions will be used.
 * @copyright Adequate Systems LLC, 2018-2024. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H


/* internal support */
#include "extlib.h"  /* for DLLIST et al */
#include "extthrd.h" /* for Mutex */
#include <errno.h>   /* for errno */

#ifdef _WIN32
   #pragma comment(lib, "ws2_32.lib")
   #include <winsock2.h>  /* for socket handling */
   #include <ws2tcpip.h>  /* for socklen_t */

   #define socket_errno             WSAGetLastError()
   #define socket_is_connected(e)   ( e == WSAEISCONN )
   #define socket_is_connecting(e)  ( e == WSAEWOULDBLOCK || e == WSAEALREADY || e == WSAEINVAL )
   #define socket_is_inuse(e)       ( e == WSAEINPROGRESS )
   #define socket_is_reset(e)       ( e == WSAECONNRESET || WSAECONNABORTED )
   #define socket_is_waiting(e)     ( e == WSAEWOULDBLOCK || e == WSAEINPROGRESS )

/* end Windows */
#else
   #include <arpa/inet.h>  /* for inet_*() et al */
   #include <netdb.h>      /* for gethostbyname() et al */
   #include <netinet/in.h> /* for sockaddr_in et all */
   #include <poll.h>       /* for pollfd, events, et al */
   #include <sys/socket.h> /* for struct sockaddr et al */
   #include <sys/un.h>     /* for struct sockaddr_un */
   #include <unistd.h>     /* for close() */

   #define closesocket(sd)          close(sd)
   #define socket_errno             errno
   #define socket_is_connected(e)   ( e == EISCONN )
   #define socket_is_connecting(e)  ( e == EINPROGRESS || e == EALREADY )
   #define socket_is_inuse(e)       ( e == EADDRINUSE )
   #define socket_is_reset(e)       ( e == ECONNRESET || e == ECONNABORTED )
   #define socket_is_waiting(e)     ( e == EAGAIN || e == EWOULDBLOCK )

   #define INVALID_SOCKET  (SOCKET) ( ~0 )
   #define SOCKET_ERROR             ( -1 )

   typedef int SOCKET;

/* end UNIX-like */
#endif

/**
 * @def closesocket(sd)
 * Close socket descriptor. Windows specifies closesocket() natively.
 *
 * @def socket_errno
 * Detailed socket error code.
 * <br/>On Windows, expands to: `WSAGetLastError()`
 * <br/>Otherwise, expands to: `errno`
 *
 * @def socket_is_connected(e)
 * Conditional check for connected socket (i.e. after connect() call).
 * <br/>On Windows, expands to: `( e == WSAEISCONN )`
 * <br/>Otherwise, expands to: `( e == EISCONN )`
 *
 * @def socket_is_connecting(e)
 * Conditional check for connecting socket.
 * <br/>On Windows, expands to:
 * `( e == WSAEWOULDBLOCK || e == WSAEALREADY || e == WSAEINVAL )`
 * <br/>Otherwise, expands to: `( e == EINPROGRESS || e == EALREADY )`
 *
 * @def socket_is_inuse(e)
 * Conditional check for already in-use sockets (i.e. after bind() call).
 * <br/>On Windows, expands to: `( e == WSAEINPROGRESS )`
 * <br/>Otherwise, expands to: `( e == EADDRINUSE )`
 *
 * @def socket_is_reset(e)
 * Conditional check for socket reset or abort.
 * <br/>On Windows, expands to: `( e == WSAECONNRESET || WSAECONNABORTED )`
 * <br/>Otherwise, expands to: `( e == ECONNRESET || e == ECONNABORTED )`
 *
 * @def socket_is_waiting(e)
 * Conditional check for waiting socket (i.e. waiting on recv/send).
 * <br/>On Windows, expands to:
 * `( e == WSAEWOULDBLOCK || e == WSAEINPROGRESS )`
 * <br/>Otherwise, expands to: `( e == EAGAIN || e == EWOULDBLOCK )`
 *
 * @def INVALID_SOCKET
 * Invalid socket descriptor value. Ideally this should be `( -1 )`, but
 * since it makes no difference to the compiler, `( ~0 )` is used so as to
 * immitate the Windows variant where the SOCKET type is `UINT_PTR`.
 *
 * @def SOCKET_ERROR
 * Error return value for most socket related functions.
 *
 * @typedef int SOCKET
 * Socket descriptor type (non-Windows ONLY). While Windows uses UINT_PTR
 * for it's SOCKET type, UNIX-like systems use integer file descriptors.
*/

/* C/C++ compatible function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

char *ntoa(void *n, char a[16]);
unsigned long aton(char *a);
int connect_auto(SOCKET sd, const struct sockaddr *addrp);
int connect_timed
   (SOCKET sd, const struct sockaddr *addrp, socklen_t len, int seconds);
int get_hostipv4(char *name, size_t namelen);
int get_hostipv6(char *name, size_t namelen);
const char *inet_ntop_auto
   (const struct sockaddr *src, char *dst, size_t size);
int recv_timed(SOCKET sd, void *pkt, int len, int flags, int seconds);
int send_timed(SOCKET sd, void *pkt, int len, int flags, int seconds);
int set_blocking(SOCKET sd);
int set_nonblocking(SOCKET sd);
int wsa_cleanup(void);
int wsa_startup(unsigned char major, unsigned char minor);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
