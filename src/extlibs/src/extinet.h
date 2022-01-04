/**
 * @file extinet.h
 * @brief Extended internet support.
 * @copyright © Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
 * @note Applications MUST call sock_startup() before using socket
 * support functions, and may optionally cleanup with sock_cleanup()
*/

#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H  /* include guard */


#ifdef _WIN32

   #pragma comment(lib, "ws2_32.lib")
   #include <winsock2.h>  /* for socket handling */
   #include <ws2tcpip.h>  /* for socklen_t */

   #define ext_sock_close(_sd)      closesocket(_sd)
   #define ext_sock_err             ( WSAGetLastError() )
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

/* end Windows */
#elif defined(__unix__)

   #include <arpa/inet.h>
   #include <errno.h>
   #include <netdb.h>
   #include <netinet/in.h>
   #include <sys/socket.h>
   #include <unistd.h>

   #define ext_sock_close(_sd)      close(_sd)
   #define ext_sock_err             ( errno )
   #define sock_err_is_success(_e)  ( _e == EISCONN )
   #define sock_err_is_waiting(_e)  ( _e == EINPROGRESS || _e == EALREADY )

/* end Unix */
#endif


/**
 * @brief Function definition to close an open socket.
 * @param _sd socket descriptor
 * @note Where `_WIN32` is defined, expands to: `closesocket(_sd)`
 * @note Where `__unix__` is defined, expands to: `close(_sd)`
 * @note DOES NOT clear socket descriptor
*/
#define sock_close(_sd)  ext_sock_close(_sd)

/**
 * @brief Function return code for socket error.
 * @note Where `_WIN32` is defined, expands to: `( WSAGetLastError() )`
 * @note Where `__unix__` is defined, expands to: `( errno )`
*/
#define sock_err  ext_sock_err


#ifndef SOCKET
/**
 * @brief SOCKET datatype for use with socket support functions
 * @note SOCKET is ONLY guaranteed an int type where NOT already defined.
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
 * @brief Global flag indicating socket support is enabled.
 * @details Increments upon use of sock_startup().
 * Returns to zero after sock_cleanup().
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
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout);
SOCKET sock_connect_addr(char *addr, unsigned short port, double timeout);
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout);
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout);

#ifdef __cplusplus
}
#endif


#endif /* end EXTENDED_INTERNET_H */
