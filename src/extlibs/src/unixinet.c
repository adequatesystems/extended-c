/**
 * unixinet.c - Unix specific extended internet support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifndef _WIN32  /* OS guard, assumes unix-like */
#ifndef UNIX_INTERNET_C
#define UNIX_INTERNET_C  /* include guard */


#include "extinet.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef SOCKET
/* Standard datatype used for SOCKET */
#define SOCKET int
#endif

/**
 * Perform a http(s) GET request to a file, fname.
 * Standard protocols and ports are automatically assigned, if omitted.
 *
 * URL format: [<protocol>://]<domain>[:<port>]/<path/to/file>
 *    e.g. https://api.mochimap.com:443/network/start.lst
 *
 * Returns 0 on success, else error code. */
int http_get(char *url, char *fname)
{
   char cmd[640];

   /* If you've got a solution for a POSIX compliant routine for
    * downloading resources from "secure" (https) locations,
    * without using libraries, take the stage... */

   /* url is required... obviously */
   if (url == NULL) return 8;

   /* try cURL */
   sprintf(cmd, "curl -sL%s %.256s %.336s >/dev/null 2>&1",
      fname ? "o" : "O", fname ? fname : "", url);
   if(system(cmd) == 0) return 0;

   /* try wGET */
   sprintf(cmd, "wget -q%s %.256s %.336s >/dev/null 2>&1",
      fname ? "O" : "", fname ? fname : "", url);
   if(system(cmd) == 0) return 0;

   return 1;
}  /* end http_get() */

/* Set socket sd to non-blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_nonblock(SOCKET sd)
{
   int flags;

   flags = fcntl(sd, F_GETFL, 0);
   return fcntl(sd, F_SETFL, flags | O_NONBLOCK);
}  /* end sock_set_nonblock() */

/* Set socket sd to blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_blocking(SOCKET sd)
{
   int flags;

   flags = fcntl(sd, F_GETFL, 0);
   return fcntl(sd, F_SETFL, flags & (~O_NONBLOCK));
}  /* end sock_set_blocking() */


#endif  /* end UNIX_INTERNET_C */
#endif  /* end OS guard */
