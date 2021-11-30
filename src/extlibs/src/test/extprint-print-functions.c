
#include "_assert.h"
#include "../extprint.h"



int main()
{
   char *inputp, input[1024];
   char *printable = " !\"#$%&'()*+,-./0123456789:;<=>?@"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
   char *expect = "Success;  !\"#$%&'()*+,-./0123456789:;<=>?@"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
   int year, month, day, hour, minute, second;

   /* check initialized value of nlogs */
   ASSERT_EQ2_MSG(Perrcfg.nlogs, Pstderrcfg.nlogs, 0,
      "nlogs should initialize Zero (0)");
   ASSERT_EQ2_MSG(Plogcfg.nlogs, Pstdoutcfg.nlogs, 0,
      "nlogs should initialize Zero (0)");
   ASSERT_EQ_MSG(Pbugcfg.nlogs, 0, "nlogs should initialize Zero (0)");

   /* check return values of print functions */
   ASSERT_EQ2_MSG(perrno(-1, NULL), 2, perrno(-1, ""),
      "perrno() should return 2 when errnum is < 0");
   ASSERT_EQ2_MSG(perrno(0, NULL), 1, perrno(0, ""),
      "perrno() should return 1 when errnum is >= 0");
   ASSERT_EQ2_MSG(perr(NULL), 1, perr(""), "perr() should return 1");
   ASSERT_EQ2_MSG(plog(NULL), 0, plog(""), "plog() should return 0");
   ASSERT_EQ2_MSG(pbug(NULL), 0, pbug(""), "pbug() should return 0");

   /* check file pointers where assigned for err/out Pconfig's */
   ASSERT_EQ_MSG(Pstderrcfg.fp, stderr, "err.fp should be assigned stderr");
   ASSERT_EQ_MSG(Pstdoutcfg.fp, stdout, "out.fp should be assigned stdout");

   /* check nlogs incremented appropriately on default settings */
   ASSERT_EQ_MSG(Perrcfg.nlogs, 0,
      "perr.nlogs should not increment on default settings");
   ASSERT_EQ_MSG(Plogcfg.nlogs, 0,
      "plog.nlogs should not increment on default settings");
   ASSERT_EQ_MSG(Pbugcfg.nlogs, 0,
      "pbug.nlogs should not increment on default settings");
   ASSERT_EQ_MSG(Pstderrcfg.nlogs, 3,
      "nlogs should have incremented to 3 on default settings");
   ASSERT_EQ_MSG(Pstdoutcfg.nlogs, 1,
      "nlogs should have incremented to 1 on default settings");

   /* (re)assign file pointers */
   Pstderrcfg.fd = 0; Pstderrcfg.fp = NULL;
   Pstdoutcfg.fd = 0; Pstdoutcfg.fp = NULL;
   Perrcfg.fp = fopen("perr.tmp", "w+");
   Perrcfg.time = 1;

   /* check print function prints to perr.tmp */
   perrno(0, "%s", printable);
   rewind(Perrcfg.fp);
   fread(input, 1, 1024, Perrcfg.fp);
   input[1023] = '\0';
   /* check timestamp format */
   ASSERT_EQ_MSG(
      sscanf(input, "%d-%02d-%02dT%02d:%02d:%02d",
         &year, &month, &day, &hour, &minute, &second),
      6, "fscanf() read incorrect number of log components");
   ASSERT_GE(year, 1900);
   ASSERT_LE2(1, month, 12);
   ASSERT_LE2(1, day, 31);
   ASSERT_LE2(0, hour, 23);
   ASSERT_LE2(0, minute, 59);
   ASSERT_LE2(0, second, 59);
   /* check default prefix is printed appropriately */
   inputp = strstr(input, Perrcfg.pre);
   ASSERT_STR(inputp, Perrcfg.pre, strlen(Perrcfg.pre));
   inputp += strlen(Perrcfg.pre); /* fast-forward input pointer */
   /* check remaining log is as expected */
   ASSERT_STR(inputp, expect, strlen(expect));

   /* cleanup */
   if (Perrcfg.fp) {
      fclose(Perrcfg.fp);
      remove("perr.tmp");
   }
}
