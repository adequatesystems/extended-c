/**
 * @file extinet.h
 * @brief Extended internet support.
 * @details Provides extended support for internet related functionality.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
 * @note Applications MUST call sock_startup() before using some
 * socket support functions in this file.
*/

#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H  /* include guard */


#include "extos.h"

#if OS_WINDOWS
   #pragma comment(lib, "ws2_32.lib")
   #include <winsock2.h>  /* for socket handling */
   #include <ws2tcpip.h>  /* for socklen_t */

   #define os_dependant_sock_errno  ( WSAGetLastError() )
   #define sock_err_is_success(_e)  ( _e == WSAEISCONN )
   #define sock_err_is_waiting(_e) \
      ( _e == WSAEWOULDBLOCK || _e == WSAEALREADY || _e == WSAEINVAL )

   #ifdef __cplusplus
   extern "C" {
   #endif

   /* Global winsock variables */
   WSADATA Sockdata;
   WORD Sockverreq;

   #ifdef __cplusplus
   }
   #endif

/* end OS_WINDOWS */
#elif OS_UNIX
   #include <arpa/inet.h>
   #include <errno.h>
   #include <netdb.h>
   #include <netinet/in.h>
   #include <sys/socket.h>
   #include <unistd.h>

   #define os_dependant_sock_errno  ( errno )
   #define sock_err_is_success(_e)  ( _e == EISCONN )
   #define sock_err_is_waiting(_e)  ( _e == EINPROGRESS || _e == EALREADY )

/* end Unix */
#endif

/**
 * Detailed socket error code.
 * <br/>On Windows, expands to: `( WSAGetLastError() )`
 * <br/>On Unix, expands to: `( errno )`
*/
#define sock_errno   os_dependant_sock_errno

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


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Global flag indicating socket support status. Increments upon
 * use of sock_startup(). Returns to zero after sock_cleanup().
*/
volatile int Sockinuse;

/* Function prototypes */
int http_get(char *url, char *fname, int timeout);
int phostinfo(void);
char *ntoa(void *n, char *a);
unsigned long aton(char *a);
unsigned long get_sock_ip(SOCKET sd);
int sock_startup(void);
int sock_cleanup(void);
int sock_set_nonblock(SOCKET sd);
int sock_set_blocking(SOCKET sd);
int sock_close(SOCKET sd);
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout);
SOCKET sock_connect_addr(char *addr, unsigned short port, double timeout);
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout);
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout);

/* end extern "C" {} for C++ */
#ifdef __cplusplus
}
#endif

/* end include guard */
#endif
