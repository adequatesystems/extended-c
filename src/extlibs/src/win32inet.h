/**
 * win32inet.h - Windows internet support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 16 September 2021
 * Revised: 12 November 2021
 *
*/

#ifdef _WIN32  /* OS guard */
#ifndef WIN32_INTERNET_H
#define WIN32_INTERNET_H  /* include guard */


#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <ws2tcpip.h>  /* for socklen_t */

#define get_sock_err()           WSAGetLastError()
#define sock_err_is_success(_e)  ( _e == WSAEISCONN )
#define sock_err_is_waiting(_e) \
   ( _e == WSAEWOULDBLOCK || _e == WSAEALREADY || _e == WSAEINVAL )
#define sock_close(_sd)          closesocket(_sd)
#define sock_sleep()             Sleep(1000)


#ifdef __cplusplus
extern "C" {
#endif


/* Global winsock variables */
WSADATA Sockdata;
WORD Sockverreq;
int Sockstarted;

/* Function prototypes */
int http_get(char *url, char *fname);
int sock_startup(void);
int sock_cleanup(void);
int sock_set_nonblock(SOCKET sd);
int sock_set_blocking(SOCKET sd);


#ifdef __cplusplus
}
#endif


#endif  /* end WIN32_INTERNET_H */
#endif  /* end _WIN32 */
