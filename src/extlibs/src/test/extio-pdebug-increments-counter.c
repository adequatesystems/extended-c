
#include "_assert.h"
#include "../extio.h"

int main()
{
   ASSERT_EQ_MSG(Ndebugs, 0, "Ndebugs should start at Zero (0)");
   pdebug(NULL); ASSERT_EQ_MSG(Ndebugs, 0, "Ndebugs should not change");
   pdebug(""); ASSERT_EQ_MSG(Ndebugs, 1, "Ndebugs should increment by 1");
}
