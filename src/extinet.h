/**
 * @file extinet.h
 * @brief Extended internet support.
 * @details Provides extended support for internet related functionality.
 * Windows applications MUST call WSAStartup() or sock_startup() (supplied
 * by this support unit) before using some socket support functions.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H


#ifdef _WIN32
   #pragma comment(lib, "ws2_32.lib")
   #define FD_SETSIZE 1024    /* for UNIX consistancy */
   #include <winsock2.h>  /* for socket handling */
   #include <ws2tcpip.h>  /* for socklen_t */

   #define os_sock_errno            ( WSAGetLastError() )
   #define os_sock_connected(_e)    ( _e == WSAEISCONN )
   #define os_sock_connecting(_e) \
      ( _e == WSAEWOULDBLOCK || _e == WSAEALREADY || _e == WSAEINVAL )
   #define os_sock_waiting(_e) \
      ( _e == WSAEWOULDBLOCK || _e == WSAEINPROGRESS )

/* end Windows */
#else
   #include <arpa/inet.h>
   #include <netdb.h>
   #include <netinet/in.h>
   #include <sys/socket.h>

   #define os_sock_errno            ( errno )
   #define os_sock_connected(_e)    ( _e == EISCONN )
   #define os_sock_connecting(_e)   ( _e == EINPROGRESS || _e == EALREADY )
   #define os_sock_waiting(_e)      ( _e == EAGAIN || _e == EWOULDBLOCK )

/* end UNIX-like */
#endif

/**
 * Detailed socket error code.
 * <br/>On Windows, expands to: `( WSAGetLastError() )`
 * <br/>On Unix, expands to: `( errno )`
*/
#define sock_errno   os_sock_errno

/**
 * Conditional check for connected socket (i.e. after connect() call).
 * <br/>On Windows, expands to: `( _e == WSAEISCONN )`
 * <br/>On Unix, expands to: `( _e == EISCONN )`
*/
#define sock_connected(e)  os_sock_connected(e)

/**
 * Conditional check for connecting socket.
 * <br/>On Windows, expands to:
 * `( _e == WSAEWOULDBLOCK || _e == WSAEALREADY || _e == WSAEINVAL )`
 * <br/>On Unix, expands to: `( _e == EINPROGRESS || _e == EALREADY )`
*/
#define sock_connecting(e) os_sock_connecting(e)

/**
 * Conditional check for waiting socket (i.e. waiting on recv/send).
 * <br/>On Windows, expands to:
 * `( _e == WSAEWOULDBLOCK || _e == WSAEINPROGRESS )`
 * <br/>On Unix, expands to: `( _e == EAGAIN || _e == EWOULDBLOCK )`
*/
#define sock_waiting(e)    os_sock_waiting(e)

#ifndef SOCKET
/**
 * SOCKET datatype for use with socket support functions
 * @note SOCKET is ONLY guaranteed an `int` type where NOT already defined.
*/
#define SOCKET int
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1)  /**< Socket error return code */
#endif

#ifndef INVALID_SOCKET
#define INVALID_SOCKET (SOCKET)(~0) /**< Indicates empty socket descriptor */
#endif

/* C/C++ compatible function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

char *ntoa(void *n, char *a);
unsigned long aton(char *a);
unsigned long get_sock_ip(SOCKET sd);
int sock_startup(void);
int sock_state(void);
int sock_cleanup(void);
int sock_set_nonblock(SOCKET sd);
int sock_set_blocking(SOCKET sd);
int sock_close(SOCKET sd);
SOCKET sock_connect_ip
   (unsigned long ip, unsigned short port, double timeout);
SOCKET sock_connect_addr(char *addr, unsigned short port, double timeout);
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout);
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout);
int gethostip(char *name, int namelen);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
