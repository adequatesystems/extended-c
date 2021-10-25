
#include <errno.h>
#include "_assert.h"
#include "../extio.h"

#define FP     Pstderrfp
#define FNAME  "timestamp.log"

int main()
{  /* check; timestamp format, and year-month-day:hour:minute:second values */
   int year, month, day, hour, minute, second;

   /* init */
   year = month = day = hour = minute = second = -1;

   /* check log file opens for writing */
   ASSERT_NE((FP = fopen(FNAME, "w+")), NULL);
   Ptimestamp = 1;

   perrno(0, "");  /* print successful error */
   rewind(FP);  /* rewind log file */
   ASSERT_EQ_MSG(
      fscanf(FP, "%d-%02d-%02dT%02d:%02d:%02d",
      &year, &month, &day, &hour, &minute, &second),
      6, "fscanf() read incorrect number of timestamp components");
   ASSERT_GE(year, 1900);
   ASSERT_LE2(1, month, 12);
   ASSERT_LE2(1, day, 31);
   ASSERT_LE2(0, hour, 23);
   ASSERT_LE2(0, minute, 59);
   ASSERT_LE2(0, second, 59);

   /* cleanup */
   if (FP) {
      fclose(FP);
      remove("timestamp.log");
   }
}
