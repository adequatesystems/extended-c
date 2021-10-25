/**
 * extsock.h - Extended socket support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 16 September 2021
 * Revised: 25 October 2021
 *
 * NOTES:
 * - to abort all "in progress" socket connections in, for example,
 *   the case of a shutdown or cleanup routine, the SockAbort flag
 *   should be set to a non-zero value.
 * - win32 applications should call sockstartup() and sockcleanup(),
 *   before and after (respectively) using socket support functions.
 *   For Cross Platform convenience, these functions are defined to
 *   do "nothing" when compiled on non-win32 systems.
 *
*/

#ifndef _EXTENDED_SOCKET_H_
#define _EXTENDED_SOCKET_H_  /* include guard */


#ifndef _WIN32  /* assume UNIXLIKE */

   #include <netinet/in.h>
   #include <sys/ioctl.h>
   #include <sys/socket.h>
   #include <unistd.h>

   #define getsockerr()                   (errno)
   #define closesocket(_sd)               close(_sd)
   #define ioctlsocket(_sd, _cmd, _arg)   ioctl(_sd, _cmd, _arg)

   #define connect_success(e)    (e == EISCONN)
   #define connect_waiting(e)    (e == EINPROGRESS || e == EALREADY)

   #define sockstartup()   do { /* nothing */ } while(0)
   #define sockcleanup()   do { /* nothing */ } while(0)

#else  /* assume Windows */

   #pragma comment(lib, "ws2_32.lib")
   #include <winsock2.h>

   #define getsockerr() WSAGetLastError()

   #define connect_success(e)  (e == WSAEISCONN)
   #define connect_waiting(e)  (e == WSAEWOULDBLOCK || \
                                 e == WSAEALREADY || e == WSAEINVAL)

   #ifdef __cplusplus
   extern "C" {
   #endif

   WSADATA SockData;
   WORD SockVerReq = 0x0202;  /* version 2.2 */
   unsigned char SockStarted;

   /* Perform socket startup routines.
    * Returns 0 on success, -1 if SockVerReq was not met or error code. */
   int SockStartup(void);

   /* Perform socket cleanup for every startup call. */
   void SockCleanup(void);

   #ifdef __cplusplus
   }
   #endif

#endif /* end Windows */


/* When <windows.h> is included into a program, the below socket
 * definitions MUST OCCUR AFTER the first "windows.h" include. */
#ifndef INVALID_SOCKET
#define INVALID_SOCKET (-1) /* represents an undefined socket */
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR (-1) /* represents a socket error, as a return code */
#endif
#ifndef SOCKET
#define SOCKET int /* represents the standard datatype used for SOCKET */
#endif

/* Initialize an outgoing connection with a char *ADDR on PORT,
 * using connectip() */
#define connectaddr(ADDR, PORT)  connectip(aton(ADDR), PORT)

#ifdef __cplusplus
extern "C" {
#endif

/* Global flag to abort any socket support function loops.
 * Set non-zero to activate. */
unsigned char SockAbort;

/* Function prototypes for extsock.c */
int nonblock(SOCKET sd);
int blocking(SOCKET sd);
unsigned long getsocketip(SOCKET sd);
SOCKET connectip(unsigned long ip, unsigned short port);
int phostinfo(void);
char *ntoa(void *n, char *a);
unsigned long aton(char *a);

#ifdef __cplusplus
}
#endif


#endif /* end _EXTENDED_SOCKET_H_ */
