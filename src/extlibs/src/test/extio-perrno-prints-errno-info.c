
#include <errno.h>
#include "_assert.h"
#include "../extio.h"

#define FP        Pstderrfp
#define FNAME     "errno.log"
#define EXPECT    "Operation not permitted"

int main()
{
   char input[256];
   size_t expectlen = strlen(EXPECT);
   int ii;

   /* check log file opens for writing */
   ASSERT_NE((FP = fopen(FNAME, "w+")), NULL);
   perrno(1, "");  /* print ERRNO information */
   rewind(FP);  /* rewind log file */
   /* read log file into input, byte-by-byte */
   for (ii = 0; !feof(FP) && !ferror(FP); ii++) {
      fread(&input[ii], sizeof(char), 1, FP);
   } input[ii] = '\0';  /* ensure input is NUL terminated */

   /* check for fread() failures */
   ASSERT_EQ_MSG(errno, 0, "fread() error");
   /* check expect error info was included in log file */
   ASSERT_STR(strstr(input, EXPECT), EXPECT, expectlen);

   /* cleanup */
   if (FP) {
      fclose(FP);
      remove(FNAME);
   }
}
