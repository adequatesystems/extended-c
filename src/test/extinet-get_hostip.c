
#include "_assert.h"
#include "../extinet.h"

int main()
{
   char ipv4addr[24] = "";
   char hostipv6[48] = "";

   /* socket initialization */
   wsa_startup(2, 2);

   ASSERT_EQ_MSG(get_hostipv4(ipv4addr, sizeof(ipv4addr)), 0, "get_hostipv4() failed");
   ASSERT_NE_MSG(*ipv4addr, '\0', "ipv4addr string is empty");
   ASSERT_NE_MSG(get_hostipv4(ipv4addr, 0), 0, "get_hostipv4() didn't fail");

   /* IPv6 Network MAY BE unreachable for some systems */
   int ipv6result = get_hostipv6(hostipv6, sizeof(hostipv6));
   if (ipv6result != 0 && errno != ENETUNREACH) {
      ASSERT_EQ_MSG(get_hostipv6(hostipv6, sizeof(hostipv6)), 0, "get_hostipv6() failed");
      ASSERT_NE_MSG(*hostipv6, '\0', "hostipv6 string is empty");
      ASSERT_NE_MSG(get_hostipv6(hostipv6, 0), 0, "get_hostipv6() didn't fail");
   }

   wsa_cleanup();
}
