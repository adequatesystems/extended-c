
#include "_assert.h"
#include "../extio.h"

int main()
{
   ASSERT_EQ_MSG(Nstdouts, 0, "Nstdouts should start at Zero (0)");
   plog(NULL); ASSERT_EQ_MSG(Nstdouts, 0, "Nstdouts should not change");
   plog(""); ASSERT_EQ_MSG(Nstdouts, 1, "Nstdouts should increment by 1");
}
