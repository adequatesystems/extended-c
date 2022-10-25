
#include "_assert.h"
#include "../extio.h"

int main()
{  /* check asnprintf() appends a format as expected */
   char output[128] = "Existing String Contents Shall Remain";
   char expected[128] = "Existing String Contents Shall Remain"
      ". (123, 1.234) Additional ";
   ASSERT_EQ(asnprintf(output, 64, ". (%d, %.3lf) %s String Truncated",
      123, 1.234f, "Additional"), 42);
   ASSERT_CMP_MSG(output, expected, 128, "output DOES NOT match expected");
}
