
#include "_assert.h"
#include "../extlib.h"
#include <time.h>
int main()
{  /* check; random numbers do not exceed WORD32_MAX*/
   word32 iterations = WORD32_MAX;

   srand32(time(NULL));
   while (iterations--) {
      ASSERT_LE_MSG(rand32(), WORD32_MAX, "rand32() returned > WORD32_MAX");
   }
}
