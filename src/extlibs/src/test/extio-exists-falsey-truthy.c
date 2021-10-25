
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check exists() returns both falsey and truthy for associated files */
   ASSERT_EQ(exists("non-existant-file.log"), 0);
   ASSERT_EQ(exists(__FILE__), 1);
}
