
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check; check and verify the validity of hyper threads indicator */
   ASSERT_NE(cpu_hyper_threads(), -1);
   if (cpu_hyper_threads()) {
      ASSERT_GT(cpu_logical_cores(), cpu_actual_cores());
   } else ASSERT_EQ(cpu_logical_cores(), cpu_actual_cores());
}
