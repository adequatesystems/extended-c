
#include <stdlib.h>
#include "_assert.h"
#include "../extio.h"

#define EMPTYFNAME   "empty.log"

int main()
{  /* check existsnz() returns both falsey and truthy for associated files */
   system("touch " EMPTYFNAME);
   ASSERT_EQ(existsnz(EMPTYFNAME), 0);
   ASSERT_EQ(existsnz(__FILE__), 1);
   remove(EMPTYFNAME);
}
