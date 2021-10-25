
#include "_assert.h"
#include "../extio.h"

int main()
{
   ASSERT_EQ_MSG(Nstderrs, 0, "Nstderrs should start at Zero (0)");
   perrno(0, NULL); ASSERT_EQ_MSG(Nstderrs, 0, "Nstderrs should not change");
   perrno(0, ""); ASSERT_EQ_MSG(Nstderrs, 1, "Nstderrs should increment by 1");
}
