
#include "../extio.h"
#include <string.h>
#include <stdlib.h>
#include <assert.h>

FILE *fp = NULL;
const char *testfile = "fbsearchtestfile";
const char *testdata = "brushfrizzhelloworld";

void test_cleanup(void) {
   fclose(fp);
   remove(testfile);
}

void test_debug(int found, const char *key, size_t len, char *buf, size_t size) {
   perror("DEBUG: fbsearch()");
   printf("DEBUG: found: %d, key: %s, len: %zu, buf: %s, size: %zu\n",
      found, key, len, buf, size);
}

void test_initialize(void) {
   size_t count;

   /* write test data to test file */
   fp = fopen(testfile, "wb");
   assert(fp != NULL && "failed to create test file");
   count = fwrite(testdata, sizeof(char), strlen(testdata), fp);
   assert(count == strlen(testdata) && "failed to write test data");

   /* reopen test file for reading */
   fclose(fp);
   fp = fopen(testfile, "rb");
   assert(fp != NULL && "failed to open test file for reading");
}

void test_fbsearch(void) {
   char result[6] = { '\0' };
   int found;

   found = fbsearch(fp, "world", 5, result, sizeof(result) - 1);
   assert(found == 1 && "failed to find key in file");
   assert(strcmp(result, "world") == 0 && "failed to read correct data");

   found = fbsearch(fp, "frizz", 5, result, sizeof(result) - 1);
   assert(found == 1 && "failed to find key in file");
   assert(strcmp(result, "frizz") == 0 && "failed to read correct data");

   found = fbsearch(fp, "brush", 5, result, sizeof(result) - 1);
   assert(found == 1 && "failed to find key in file");
   assert(strcmp(result, "brush") == 0 && "failed to read correct data");

   found = fbsearch(fp, "bobby", 5, result, sizeof(result) - 1);
   assert(found == 0 && "failed to return not found");
}

void test_fbsearch_invalid_parameters(void) {
   char result[6] = { '\0' };
   int found;

   found = fbsearch(NULL, "world", 5, result, sizeof(result) - 1);
   assert(found == 0 && "failed to handle invalid file pointer");

   found = fbsearch(fp, NULL, 5, result, sizeof(result) - 1);
   assert(found == 0 && "failed to handle invalid key pointer");

   found = fbsearch(fp, "world", 0, result, sizeof(result) - 1);
   assert(found == 0 && "failed to handle invalid key length");

   found = fbsearch(fp, "world", 5, NULL, sizeof(result) - 1);
   assert(found == 0 && "failed to handle invalid buffer pointer");

   found = fbsearch(fp, "world", 5, result, 0);
   assert(found == 0 && "failed to handle invalid buffer size");
}

int main() {
   test_initialize();

   test_fbsearch();
   test_fbsearch_invalid_parameters();

   test_cleanup();
}