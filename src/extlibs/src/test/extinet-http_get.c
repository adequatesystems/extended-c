
#include "_assert.h"
#include "../extinet.h"
#include <stdio.h>

int main()
{  /* check; GET request with NULL/bogus url, with/without fname specified */
   ASSERT_GT(http_get(NULL, NULL), 0);
   ASSERT_GT(http_get("", NULL), 0);
   ASSERT_GT(http_get("https://invalid@domain.name", NULL), 0);
   ASSERT_EQ(http_get("https://www.mochimap.net/startnodes.lst", NULL), 0);
   ASSERT_EQ(remove("startnodes.lst"), 0);
   ASSERT_EQ(http_get("google.com", "google.html"), 0);
   ASSERT_EQ(remove("google.html"), 0);
}
