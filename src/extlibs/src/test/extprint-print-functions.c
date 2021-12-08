
#include "_assert.h"
#include "../extprint.h"

int main()
{
   FILE *fp;
   char *prefix = "Prefix. ";
   char *inputp, input[1024];
   char *printable = " !\"#$%&'()*+,-./0123456789:;<=>?@"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
   char *expect = " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEF"
      "GHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~: Success";
   int year, month, day, hour, minute, second;

   /* set print file pointers */
   ASSERT_NE_MSG((fp = fopen("log.tmp", "w+")), NULL, "fopen() must succeed");
   set_perr_fp(fp);
   set_plog_fp(fp);
   set_pdebug_fp(fp);
   /* set print prefixes */
   set_perr_prefix(prefix);
   set_plog_prefix(prefix);
   set_pdebug_prefix(prefix);
   /* ensure maximum print level and timestamps enabled */
   set_print_level(PLEVEL_DEBUG);
   set_print_timestamp(1);

   /* check initialized value of nlogs */
   ASSERT_EQ_MSG(get_perr_counter(), 0,
      "ERR level print counter should initialize Zero (0)");
   ASSERT_EQ_MSG(get_plog_counter(), 0,
      "LOG level print counter should initialize Zero (0)");
   ASSERT_EQ_MSG(get_pdebug_counter(), 0,
      "DEBUG level print counter should initialize Zero (0)");

   /* check return values of print functions */
   ASSERT_EQ2_MSG(perrno(-1, NULL), 2, perrno(-1, ""),
      "perrno() should return 2 when errnum is < 0, regardless of format");
   ASSERT_EQ2_MSG(perrno(0, NULL), 1, perrno(0, ""),
      "perrno() should return 1 when errnum is >= 0, regardless of format");
   ASSERT_EQ2_MSG(perr(NULL), 1, perr(""),
      "perr() should return 1, regardless of format");
   ASSERT_EQ2_MSG(plog(NULL), 0, plog(""),
      "plog() should return 0, regardless of format");
   ASSERT_EQ2_MSG(pdebug(NULL), 0, pdebug(""),
      "pdebug() should return 0, regardless of format");

   /* check nlogs incremented appropriately on default settings */
   ASSERT_EQ_MSG(get_perr_counter(), 3,
      "ERR level counter should increment");
   ASSERT_EQ_MSG(get_plog_counter(), 1,
      "LOG level counter should increment");
   ASSERT_EQ_MSG(get_pdebug_counter(), 1,
      "DEBUG level counter should increment");

   /* reduce noise and rewind fp */
   set_print_level(PLEVEL_NONE);
   rewind(fp);

   /* check print function prints to perr.tmp */
   perrno(0, "%s", printable);
   rewind(fp);
   fread(input, 1, 1024, fp);
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
   inputp = strstr(input, prefix);
   ASSERT_STR(inputp, prefix, strlen(prefix));
   inputp += strlen(prefix); /* fast-forward input pointer */
   /* check remaining log is as expected */
   ASSERT_STR(inputp, expect, strlen(expect));

   /* cleanup */
   fclose(fp);
   remove("log.tmp");
}
