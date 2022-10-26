
#include "_assert.h"
#include "extstring.h"

#include "exterrno.h"
#include <stdio.h>

int main()
{  /* check; strerror_ext() returns error strings */
   char err[BUFSIZ] = { 0 };

   ASSERT_EQ(strerror_ext(123456789, err, BUFSIZ), err);
   ASSERT_NE(err[0], 0);

   ASSERT_EQ(strerror_ext(0, err, BUFSIZ), err);
   ASSERT_NE(err[0], 0);
}
