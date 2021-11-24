/**
 * extinet.c - Extended internet support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifndef EXTENDED_INTERNET_C
#define EXTENDED_INTERNET_C  /* include guard */


#include "extinet.h"
#include <stdio.h>
#include <string.h>
#include <time.h>


/* System specific preconfiguration */
#ifdef _WIN32  /* assume Windows */

   #pragma comment(lib, "winhttp.lib")
   #define HTTPBUFFLEN  1024     /* 1KiB default http_get() buff length */
   #define WIN32_LEAN_AND_MEAN   /* minimal windows.h include */
   #include <windows.h>
   #include <winhttp.h>

   WORD Sockverreq = 0x0202;     /* Default winsock version 2.2 */

#else  /* assume UNIXLIKE */

   #include <fcntl.h>   /* for fcntl() */
   #include <stdlib.h>  /* for system() */

#endif  /* end preconfiguration */


/**
 * Perform a http(s) GET request to a file, fname.
 * Returns 0 on success, else error code. */
int http_get(char *url, char *fname)
{
#ifdef _WIN32  /* assume Windows */
   static const char *default_fname = "index.html";
   HINTERNET hSession, hConnect, hRequest;
   wchar_t wcdomain[64], wcpath[256];
   DWORD bytesAvailable, bytesRead;
   BOOL bResults;
   FILE *fp;

   /* INTERNET_DEFAULT_PORT evaluates to 80 for HTTP and 443 for HTTPS */
   unsigned int port = INTERNET_DEFAULT_PORT;
   char proto[16] = "https";
   char domain[64] = "\0";
   char path[256] = "/";
   char buf[HTTPBUFFLEN];
   char *cp;

   /* url is required... obviously */
   if (url == NULL) return 8;

   /* 
   * Standard protocols and ports are automatically assigned, if omitted.
   * URL format: [<protocol>://]<domain>[:<port>]/<path/to/file>
   * example url component extraction (in no real order)...
   *     e.g. https://example-domain.tld:443/path/to/file.ext
   * proto = "https";             port = 443;
   *        domain = "example-domain.tld";  fname = "file.ext";
   *                                path = "/path/to/file.ext"; */

   /* extract protocol (if specified), domain(:port) and path from url */
   if (strstr(url, "://")) {
      sscanf(url, "%15[^:/]://%63[^/]%255s", proto, domain, path);
   } else sscanf(url, "%63[^/]%256s", domain, path);
   /* verify necessary components */
   if (!domain[0] || !path[0]) return 3;
   /* check for, and isolate port from domain, if present */
   cp = strchr(domain, ':');
   if (cp != NULL && cp < &domain[strlen(domain) - 1]) {
      cp = strtok(domain, ":");
      port = atoi(&domain[strlen(cp) + 1]);
   }
   /* if no filename was provided... */
   if (fname == NULL) {
      /* ... extract from path or use static default */
      cp = strrchr(path, '/');
      if (cp != NULL && cp < &path[strlen(path) - 1]) {
         fname = cp + 1;
      } else fname = default_fname;
   }
   /* convert domain and path to wchar string */
   mbtowcs(wcdomain, domain, 64);
   mbtowcs(wcpath, path, 256);

   /* open file for writing */
   fp = fopen(fname, "wb");
   if (fp == NULL) return 4;

   /* init internet handles */
   bResults = FALSE;
   hSession = hConnect = hRequest = NULL;
   /* open http session */
   hSession = WinHttpOpen(L"Adequate Systems, LLC. Software",
      WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME,
      WINHTTP_NO_PROXY_BYPASS, 0);
   /* open http connection */
   if (hSession) hConnect = WinHttpConnect(hSession, domain, port, 0);
   /* open http request */
   if (hConnect) {
      hRequest = WinHttpOpenRequest(hConnect, L"GET", path, NULL,
         WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
         strncmp(proto, "https", sizeof(proto)) ? WINHTTP_FLAG_SECURE : 0);
   }
   /* send request */
   if (hRequest) {
      bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS,
         0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
      if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
   }
   /* retrieve data */
   if (bResults) {
      do {
         bytesAvailable = 0;
         WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
         /* limit bytes to size of buf */
         if(bytesAvailable > HTTPBUFFLEN) bytesAvailable = HTTPBUFFLEN;
         /* read data into buf and write buf to file */
         if(WinHttpReadData(hRequest, buf, bytesAvailable, &bytesRead)) {
            if(fwrite(buf, (size_t) bytesRead, 1, fp) != 1) break;
         }
      } while (bytesAvailable > 0);
   }

   /* cleanup */
   if (hRequest) WinHttpCloseHandle(hRequest);
   if (hConnect) WinHttpCloseHandle(hConnect);
   if (hSession) WinHttpCloseHandle(hSession);
   fclose(fp);

   /* delete partial file downloads */
   if (bytesAvailable > 0) {
      remove(fname);
      return 1;
   }

   return 0;
#else  /* assume UNIXLIKE */
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
#endif  /* end UNIXLIKE */
}  /* end http_get() */

/**
 * Print local host info on stdout.
 * Returns 0 on succesful operation, or -1 on error. */
int phostinfo(void)
{
   int result, i;
   char hostname[100], *addrstr;
   struct hostent *host = NULL;

   /* get local machine name and IP address */
   result = gethostname(hostname, sizeof(hostname));
   if(result == 0) host = gethostbyname(hostname);
   printf("Local Machine Info\n");
   if (host) {
      printf("  Machine name: %s\n", host->h_name ? host->h_name : "unknown");
      for (i = 0; host->h_aliases[i]; i++) {
         printf("     alt. name: %s\n", host->h_aliases[i]);
      }
      for (i = 0; host->h_addr_list[i]; i++) {
         addrstr = inet_ntoa(*((struct in_addr *) (host->h_addr_list[i])));
         if (host->h_addrtype == AF_INET) {
            printf("  IPv4 address: %s\n", addrstr);
         } else printf("  Unknown address: %s\n", addrstr);
      }
   } else printf("  Machine name: unknown\n  IPv4 address: unknown\n");

   printf("\n");
   return 0;
}  /* end phostinfo() */

/**
 * Convert IPv4 from 32-bit binary form to numbers-and-dots notation.
 * Place in char *a if provided, else use static char *cp.
 * Returns char* to converted result. */
char *ntoa(void *n, char *a)
{
   static char cp[16];
   unsigned char *bp;

   bp = (unsigned char *) n;
   if (a == NULL) a = cp;
   sprintf(a, "%d.%d.%d.%d", bp[0], bp[1], bp[2], bp[3]);

   return a;
}  /* end ntoa() */

/**
 * Perform an ip address lookup on the network address string *a.
 * Returns a network ip on success, or 0 on error.
 * NOTES:
 * - *a can be a hostname or numbers-and-dots notation IPv4. */
unsigned long aton(char *a)
{
   struct hostent *host;
   struct sockaddr_in addr;

   if(a == NULL) return 0;
   memset(&addr, 0, sizeof(addr));

   host = gethostbyname(a);
   if(host == NULL) return 0;
   memcpy(&(addr.sin_addr.s_addr), host->h_addr_list[0], host->h_length);

   return addr.sin_addr.s_addr;
}  /* end aton() */

/**
 * Obtain an ip address from a valid SOCKET descriptor.
 * Returns a network ip on success, or INVALID_SOCKET on error,
 * and a specific error code can be retrieved with get_sock_err(). */
unsigned long get_sock_ip(SOCKET sd)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;

   if(getpeername(sd, (struct sockaddr *) &addr, &addrlen) == 0) {
      return addr.sin_addr.s_addr;  /* the unsigned long ip */
   }

   return (unsigned long) SOCKET_ERROR;
}  /* end get_sock_ip() */

/**
 * Perform Socket Startup (ideally during main() initialization).
 * Returns 0 on success, else if _WIN32 is defined:
 * - (-1) if Sockverreq was not met
 * - WSAStartup() error code */
int sock_startup(void)
{
#ifdef _WIN32  /* assume Windows */
   /* perform startup request */
   int ecode = WSAStartup(Sockverreq, &Sockdata);
   if (ecode) return ecode;
#endif  /* end Windows */

   Sockinuse++;  /* increment to enable sockets support */

#ifdef _WIN32  /* assume Windows */
   /* ensure requested version was obtained */
   if (Sockdata.wVersion != Sockverreq) {
      return -1;
   }
#endif  /* end Windows */

   return 0;
}  /* end sock_startup() */

/**
 * Perform Winsock Cleanup for every call to sock_startup().
 * Returns 0. */
int sock_cleanup(void)
{
   while (Sockinuse--) {

#ifdef _WIN32  /* assume Windows */
      WSACleanup();
#endif  /* end Windows */

   }

   return Sockinuse;
}  /* end sock_cleanup() */

/**
 * Set socket sd to non-blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_nonblock(SOCKET sd)
{
#ifdef _WIN32  /* assume Windows */
   u_long arg = 1L;
   return ioctlsocket(sd, FIONBIO, &arg);
#else  /* assume UNIXLIKE */
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) | O_NONBLOCK);
#endif  /* end UNIXLIKE */
}  /* end sock_set_nonblock() */

/**
 * Set socket sd to blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_blocking(SOCKET sd)
{
#ifdef _WIN32  /* assume Windows */
   u_long arg = 0L;
   return ioctlsocket(sd, FIONBIO, &arg);
#else  /* assume UNIXLIKE */
   return fcntl(sd, F_SETFL, fcntl(sd, F_GETFL, 0) & (~O_NONBLOCK));
#endif  /* end UNIXLIKE */
}  /* end sock_set_blocking() */

/**
 * Initialize an outgoing connection with ip on port.
 * Returns non-blocking SOCKET on success, or INVALID_SOCKET on error.
 * NOTE: sock_startup() MUST be called to enable socket support. */
SOCKET sock_connect_ip(unsigned long ip, unsigned short port, double timeout)
{
   static socklen_t addrlen = (socklen_t) sizeof(struct sockaddr_in);
   struct sockaddr_in addr;
   time_t start;
   SOCKET sd;
   int ecode;

   if (!Sockinuse) return INVALID_SOCKET;

   sd = socket(AF_INET, SOCK_STREAM, 0);  /* AF_INET = IPv4 */
   if(sd == INVALID_SOCKET) return INVALID_SOCKET;

   memset((char *) &addr, 0, sizeof(addr));
   addr.sin_addr.s_addr = ip;
   addr.sin_family = AF_INET;  /* AF_UNIX */
   /* Convert short integer to network byte order */
   addr.sin_port = htons(port);

   sock_set_nonblock(sd);
   time(&start);

   while (connect(sd, (struct sockaddr *) &addr, addrlen)) {
      ecode = sock_err;
      if (sock_err_is_success(ecode)) break;
      if (sock_err_is_waiting(ecode) && Sockinuse &&
         (difftime(time(NULL), start) < timeout)) {
         sock_sleep();  /* socket is waiting patiently */
         continue;
      }  /* ... connection is deemed a failure, cleanup... */
      sock_close(sd);
      return INVALID_SOCKET;
   }

   return sd;
}  /* end sock_connect_ip() */

/**
 * Receive a packet of data from SOCKET sd into pkt[len].
 * Timeout is ignored if socket is set to blocking.
 * Returns:
 * - (0) for success
 * - (1) for end communication
 * - (-1) for timeout */
int sock_recv(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) recv(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0 || !Sockinuse) return 1;  /* end communication */
      if (count > 0) n += count;  /* count recv'd bytes */
      else if(difftime(time(NULL), start) >= timeout) return (-1);
      else sock_sleep();  /* socket is waiting patiently */
   }

   return 0;  /* recv'd packet */
}  /* end sock_recv() */

/**
 * Send a packet of data on SOCKET sd from pkt[len].
 * Timeout is ignored if socket is set to blocking.
 * Returns:
 * - (0) for success
 * - (1) for end communication
 * - (-1) for timeout */
int sock_send(SOCKET sd, void *pkt, int len, int flags, double timeout)
{
   int n, count;
   time_t start;

   time(&start);
   for(n = 0; n < len; ) {
      /* check socket for pending data */
      count = (int) send(sd, ((char *) pkt) + n, len - n, flags);
      if (count == 0 || !Sockinuse) return 1;  /* end communication */
      if (count > 0) n += count;  /* count sent bytes */
      else if(difftime(time(NULL), start) >= timeout) return (-1);
      else sock_sleep();  /* socket is waiting patiently */
   }

   return 0;  /* sent packet */
}  /* end sock_send() */


#endif  /* end EXTENDED_INTERNET_C */
