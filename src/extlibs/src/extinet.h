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
 * - to abort all "in progress" inet connections in, for example,
 *   the case of a shutdown or cleanup routine, the Shutdown flag
 *   should be set to a non-zero value.
 * - where applications are designed for compatibility with win32
 *   systems, call sock_startup() and sock_cleanup(), in the init
 *   and the cleanup routines of an application.
 *
*/

#ifndef EXTENDED_INTERNET_H
#define EXTENDED_INTERNET_H  /* include guard */


#ifdef _WIN32  /* OS includes ... */
#include "win32inet.h"
#else
#include "unixinet.h"
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

/* Create a cokect connection with an addres, *_addr, on port, _port. */
#define sock_connect_addr(_addr, _port, _timeout) \
   sock_connect_ip(aton(_addr), _port, _timeout)


#ifdef __cplusplus
extern "C" {
#endif

/* Global flag to abort any inet support function loops.
 * Set non-zero to activate. */
int Shutdown;

/* OS specific function prototypes */
int http_get(char *url, char *fname);
int sock_set_nonblock(SOCKET sd);
int sock_set_blocking(SOCKET sd);

/* Function prototypes */
int phostinfo(void);
char *ntoa(void *n, char *a);
unsigned long aton(char *a);
unsigned long get_sock_ip(SOCKET sd);
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout);
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout);
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout);

#ifdef __cplusplus
}
#endif


#endif /* end EXTENDED_INTERNET_H */
