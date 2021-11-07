
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check; CPU logical cores is greater than zero */
   ASSERT_GT(cpu_logical_cores(), 0);
}
