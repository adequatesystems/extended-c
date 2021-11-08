
#include "_assert.h"
#include "../extio.h"
#include <time.h>

int main()
{  /* this test simply tests basic operation without issue */
   char prog_name[] = "Progress";
   char prog_name2[] = "Progress2";
   time_t last, now, then;
   double i;

   last = time(&then);
   Pconsole = PCONSOLE_LOG;
   /* print progress bar over 5 seconds, lazy spin loops */
   pprog(prog_name, "B", 0, 0);
   pprog(prog_name2, NULL, 0, 5);
   for (i = 0; i < 5; time(&now), i = difftime(now, then)) {
      if (difftime(now, last)) {
         pprog(prog_name, NULL, i, 0);
         pprog(prog_name2, NULL, i, 5);
      }
   }
   pprog_done(prog_name);
   pprog_done(prog_name2);
}
