
#include "_assert.h"
#include "../extio.h"
#include <string.h>

int main()
{  /* check; CPU Vendor/Brand does not return an empty string */
   ASSERT_NE(strncmp(cpu_vendor(), "", 12), 0);
   ASSERT_NE(strncmp(cpu_brand(), "", 48), 0);
}
