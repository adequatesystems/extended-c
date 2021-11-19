/**
 * win32inet.c - Windows internet support
 *
 * Copyright (c) 2018-2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE *** NO WRRANTY ***
 *
 * Date: 9 January 2018
 * Revised: 12 November 2021
 *
*/

#ifdef _WIN32  /* OS guard */
#ifndef WIN32_INTERNET_C
#define WIN32_INTERNET_C  /* include guard */


#include "extinet.h"

#pragma comment(lib, "winhttp.lib")
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winhttp.h>
#include <stdio.h>

/* default HTTP buff length of 1KiB */
#define HTTPBUFFLEN  1024

/* Default winsock version 2.2 */
WORD Sockverreq = 0x0202;

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

   /* example url component extraction (in no real order)...
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
   if (!SockAbort && hSession) hConnect = WinHttpConnect(hSession, domain, port, 0);
   /* open http request */
   if (!SockAbort && hConnect) {
      hRequest = WinHttpOpenRequest(hConnect, L"GET", path, NULL,
         WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES,
         strncmp(proto, "https", sizeof(proto)) ? WINHTTP_FLAG_SECURE : 0);
   }
   /* send request */
   if (!SockAbort && hRequest) {
      bResults = WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS,
         0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
      if (bResults) bResults = WinHttpReceiveResponse(hRequest, NULL);
   }
   /* retrieve data */
   if (!SockAbort && bResults) {
      do {
         bytesAvailable = 0;
         WinHttpQueryDataAvailable(hRequest, &bytesAvailable);
         /* limit bytes to size of buf */
         if(bytesAvailable > HTTPBUFFLEN) bytesAvailable = HTTPBUFFLEN;
         /* read data into buf and write buf to file */
         if(WinHttpReadData(hRequest, buf, bytesAvailable, &bytesRead)) {
            if(fwrite(buf, (size_t) bytesRead, 1, fp) != 1) break;
         }
      } while (!SockAbort && bytesAvailable > 0);
   }

   /* cleanup */
   if(hRequest) WinHttpCloseHandle(hRequest);
   if(hConnect) WinHttpCloseHandle(hConnect);
   if(hSession) WinHttpCloseHandle(hSession);
   fclose(fp);

   /* delete partial file downloads */
   if (bytesAvailable > 0) {
      remove(fname);
      return 1;
   }

   return 0;
}  /* end http_get() */

/**
 * Perform Winsock Startup. Returns 0 on success,
 * -1 if SockVerReq was not met, or other error code. */
int sock_startup(void)
{
   int ecode;

   /* perform startup request */
   ecode = WSAStartup(Sockverreq, &Sockdata);
   if (ecode == 0) Sockstarted++;
   else return ecode;
   /* ensure requested version was obtained */
   if (Sockdata.wVersion != Sockverreq) {
      return -1;
   }

   return 0;
}  /* end sock_startup() */

/**
 * Perform Winsock Cleanup for every call to sockstartup().
 * Returns 0 on success, else current value of Sockstarted. */
int sock_cleanup(void)
{
   while (Sockstarted--) WSACleanup();

   return Sockstarted;
}  /* end sock_cleanup() */

/**
 * Set socket sd to non-blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_nonblock(SOCKET sd)
{
   u_long arg = 1L;

   return ioctlsocket(sd, FIONBIO, &arg);
}  /* end sock_set_nonblock() */

/**
 * Set socket sd to blocking I/O.
 * Returns 0 on success, or SOCKET_ERROR on error, and
 * a specific error code can be retrieved with getsockerr(). */
int sock_set_blocking(SOCKET sd)
{
   u_long arg = 0L;

   return ioctlsocket(sd, FIONBIO, &arg);
}  /* end sock_set_blocking() */


#endif  /* end WIN32_INTERNET_C */
#endif  /* end _WIN32 */
