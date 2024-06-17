
#include "_assert.h"
#include "../extinet.h"

/* reference addresses */
#define CLOUDFLARE_DNS_IPv4 "1.1.1.1"
#define CLOUDFLARE_DNS_IPv6 "2606:4700:4700::1111"

int main()
{
   struct sockaddr addr_un = { .sa_family = AF_UNIX, { 0 } };
   struct sockaddr_in6 addr6 = {
      .sin6_family = AF_INET6, .sin6_addr.s6_addr = {
         0x26, 0x6, 0x47, 0, 0x47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x11, 0x11
      }, 0
   };
   struct sockaddr_in addr = {
      .sin_family = AF_INET, .sin_addr.s_addr = 0x01010101
   };
   char buffer[128] = "\0";

   ASSERT_STR(
      inet_ntop_auto((struct sockaddr *) &addr, buffer, sizeof(buffer)),
      CLOUDFLARE_DNS_IPv4, sizeof(CLOUDFLARE_DNS_IPv4));
   ASSERT_STR(
      inet_ntop_auto((struct sockaddr *) &addr6, buffer, sizeof(buffer)),
      CLOUDFLARE_DNS_IPv6, sizeof(CLOUDFLARE_DNS_IPv6));
   ASSERT_EQ(inet_ntop_auto((struct sockaddr *) &addr_un, buffer, sizeof(buffer)), NULL);
   ASSERT_EQ(errno, EAFNOSUPPORT);
}