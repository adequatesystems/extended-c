/**
 * extinet.h - Extended internet support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 16 September 2021
 * Revised: 12 November 2021
 *
 * NOTES:
 * - where applications are designed for compatibility with win32
 *   systems, call sock_startup() and sock_cleanup(), in the init
 *   and the cleanup routines of an application.
 *
*/

#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H  /* include guard */


#ifdef _WIN32  /* assume Windows */

   #pragma comment(lib, "ws2_32.lib")
   #include <winsock2.h>  /* for socket handling */
   #include <ws2tcpip.h>  /* for socklen_t */

   #define sock_err                 WSAGetLastError()
   #define sock_err_is_success(_e)  ( _e == WSAEISCONN )
   #define sock_err_is_waiting(_e) \
      ( _e == WSAEWOULDBLOCK || _e == WSAEALREADY || _e == WSAEINVAL )
   #define sock_close(_sd)          closesocket(_sd)
   #define sock_sleep()             Sleep(1)

   #ifdef __cplusplus
   extern "C" {
   #endif

   /* Global winsock variables */
   WSADATA Sockdata;
   WORD Sockverreq;

   /* Function prototypes */
   int http_get(char *url, char *fname);

   #ifdef __cplusplus
   }
   #endif

#else  /* end Windows, assume Unix-like */

   #include <arpa/inet.h>
   #include <errno.h>
   #include <netdb.h>
   #include <netinet/in.h>
   #include <sys/socket.h>
   #include <unistd.h>

   #define sock_err                 errno
   #define sock_err_is_success(_e)  ( _e == EISCONN )
   #define sock_err_is_waiting(_e)  ( _e == EINPROGRESS || _e == EALREADY )
   #define sock_close(_sd)          close(_sd)
   #define sock_sleep()             usleep(1000)

#endif


#ifndef SOCKET
/* Standard datatype used for SOCKET */
#define SOCKET int
#endif

#ifndef SOCKET_ERROR
/* Standard socket error, as a return code */
#define SOCKET_ERROR (-1)
#endif

#ifndef INVALID_SOCKET
/* Standard undefined socket */
#define INVALID_SOCKET (SOCKET)(~0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

/* Global flag indicating sockets are in use. */
volatile int Sockinuse;

/* Function prototypes */
int http_get(char *url, char *fname);
int phostinfo(void);
char *ntoa(void *n, char *a);
unsigned long aton(char *a);
unsigned long get_sock_ip(SOCKET sd);
int sock_startup(void);
int sock_cleanup(void);
int sock_set_nonblock(SOCKET sd);
int sock_set_blocking(SOCKET sd);
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout);
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout);
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout);

#ifdef __cplusplus
}
#endif


#endif /* end EXTENDED_INTERNET_H */
