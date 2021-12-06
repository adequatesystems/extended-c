
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check; CPU actual cores is greater than zero */
   ASSERT_GT(cpu_actual_cores(), 0);
}
