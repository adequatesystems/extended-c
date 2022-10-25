
#include "_assert.h"
#include "../extlib.h"

#include <stdio.h>

#define SORTSZ ( 32LL )

#define FNAME  "random.dat"
/* NOTE: FSIZE MUST USE AN ODD COUNT TO TEST VARIOUS MERGE CONDITIONS */
#define FSIZE  ( (SORTSZ * 1234567) ) /* ~38M */
#define BUFSZ  ( 1LL << 20 ) /* 1M */

int comp(const void *a, const void *b)
{
   return memcmp(a, b, SORTSZ);
}

int main()
{
   FILE *fp;
   void *buf;
   char buf1[SORTSZ];
   char buf2[SORTSZ];
   size_t i, idx;

   /* failure checks */
   ASSERT_EQ(filesort(NULL, SORTSZ, BUFSZ, comp), EOF);
   ASSERT_EQ(filesort(FNAME, SORTSZ, BUFSZ, NULL), EOF);
   ASSERT_EQ(filesort(FNAME, 0, BUFSZ, comp), EOF);
   ASSERT_EQ(filesort(FNAME, SORTSZ, 0, comp), EOF);
   ASSERT_EQ(filesort("dummy.file", SORTSZ, BUFSZ, comp), EOF);

   /* write random data to file */
   ASSERT_NE((buf = malloc(FSIZE)), NULL);
   ASSERT_NE((fp = fopen(FNAME, "wb")), NULL);
   for (idx = 0; idx < FSIZE; idx += sizeof(int)) {
      *(int *) ((char *) buf + idx) = rand();
   }
   ASSERT_EQ(fwrite(buf, FSIZE, 1, fp), 1);
   fclose(fp);
   free(buf);

   int result = filesort(FNAME, SORTSZ, BUFSZ, comp);
   if (result) perror("fsort()");
   ASSERT_EQ_MSG(result, 0, "fsort() FAILURE");

   ASSERT_NE((fp = fopen(FNAME, "rb")), NULL);
   ASSERT_EQ(fread(buf1, SORTSZ, 1, fp), 1);
   for (i = SORTSZ; ; i += SORTSZ) {
      if (fread(buf2, SORTSZ, 1, fp) != 1) {
         ASSERT_EQ_MSG(ferror(fp), 0, "IO ERROR");
         break;
      }
      int cmp = comp(buf1, buf2);
      if (cmp > 0) printf("sort check failed at i= %zu\n", i);
      ASSERT_LE_MSG(cmp, 0, "bad sort");
      memcpy(buf1, buf2, SORTSZ);
   }
   fclose(fp);
   ASSERT_EQ_MSG(i, FSIZE, "unexpected file size");

   remove(FNAME);
}
