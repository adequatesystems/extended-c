
#include "_assert.h"
#include "../extio.h"
#include <errno.h>

void cleanup(void)
{
   remove("tmp/dir/tree/test3.tmp");
   remove("tmp/dir/tree/test2.tmp");
   remove("tmp/dir/tree/test.tmp");
   remove("tmp/dir/tree");
   remove("tmp/dir");
   remove("tmp");
}

int main()
{
   char input[1024], nametoolong[FILENAME_MAX + 1];
   char *printable = " !\"#$%&'()*+,-./0123456789:;<=>?@"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
   unsigned i;
   FILE *fp;

   /* cleanup previous failures -- it's sensitive */
   cleanup();

   /* check mkdir_p() fault conditions */
   for (i = 0; i < sizeof(nametoolong); i++) nametoolong[i] = 'A';
   ASSERT_NE(mkdir_p(NULL), 0);
   ASSERT_EQ(errno, EINVAL);
   ASSERT_NE(mkdir_p(nametoolong), 0);
   ASSERT_EQ(errno, ENAMETOOLONG);
   ASSERT_NE(mkdir_p(__FILE__"/abc"), 0);
   /* check ftouch fault conditions */
   ASSERT_NE(ftouch("tmp/dir/tree/test.tmp"), 0);
   /* check fcopy() source fault conditions */
   ASSERT_NE(fcopy(NULL, NULL), 0);
   ASSERT_NE(fcopy("tmp/dir/tree/test.tmp", NULL), 0);
   /* create directory structure and touch a file within directory */
   ASSERT_EQ_MSG(mkdir_p("tmp/dir/tree"), 0, "mkdir_p() should return 0");
   ASSERT_EQ_MSG(mkdir_p("tmp/dir/tree"), 0, "mkdir_p() shouldn't complain");
   ASSERT_EQ(ftouch("tmp/dir/tree/test.tmp"), 0);
   ASSERT_EQ(fexists("tmp/dir/tree/test.tmp"), 1);
   /* write data to file for copy test */
   fp = fopen("tmp/dir/tree/test.tmp", "w");
   fwrite(printable, strlen(printable), 1, fp);
   fclose(fp);
   fp = NULL;
   /* check fcopy() destination fault conditions */
   ASSERT_NE(fcopy("tmp/dir/tree/test.tmp", NULL), 0);
   ASSERT_NE(fcopy("tmp/dir/tree/test.tmp", "tmp/dir/tree/bad/file"), 0);
   /* copy file to same location */
   ASSERT_EQ(fcopy("tmp/dir/tree/test.tmp", "tmp/dir/tree/test2.tmp"), 0);
   ASSERT_EQ(fexists("tmp/dir/tree/test2.tmp"), 1);
   /* check fsave() capabilities */
   fp = fopen("tmp/dir/tree/test2.tmp", "rb");
   ASSERT_EQ(fsave(fp, "tmp/dir/tree/test3.tmp"), 0);
   fclose(fp);
   ASSERT_EQ(fexists("tmp/dir/tree/test3.tmp"), 1);
   /* check copied data is exact */
   fp = fopen("tmp/dir/tree/test3.tmp", "r");
   fread(input, 1, sizeof(input), fp);
   fclose(fp);
   ASSERT_STR(input, printable, strlen(printable));

   /* cleanup */
   cleanup();
}
