
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check; CPU cache is greater than zero */
   ASSERT_GT(cpu_cache(), 0);
}
