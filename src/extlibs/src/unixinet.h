/**
 * unixinet.h - Unix internet support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 16 September 2021
 * Revised: 12 November 2021
 *
*/

#ifndef _WIN32  /* OS guard, assumes unix-like */
#ifndef UNIX_INTERNET_H
#define UNIX_INTERNET_H  /* include guard */


#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#define get_sock_err()           ( errno )
#define sock_err_is_success(_e)  ( _e == EISCONN )
#define sock_err_is_waiting(_e)  ( _e == EINPROGRESS || _e == EALREADY )
#define sock_close(_sd)          close(_sd)
#define sock_sleep()             usleep(1000)
#define sock_startup()           do { /* nothing */ } while(0)
#define sock_cleanup()           do { /* nothing */ } while(0)


#endif  /* end UNIX_INTERNET_H */
#endif  /* end OS guard */
