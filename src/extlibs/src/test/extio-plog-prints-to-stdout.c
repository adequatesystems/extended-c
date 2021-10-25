
#include "../extio.h"

#define FP           stdout
#define FNAME        "stdout.log"
#define CONSOLE      PCONSOLE_LOG
#define PREFIX       PPREFIX_LOG
#define PFUNC(...)   plog(__VA_ARGS__)

#include "_main-prints-to-file.h"
