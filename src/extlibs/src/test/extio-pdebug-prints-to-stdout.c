
#include "../extio.h"

#define FP           stdout
#define FNAME        "stdout.log"
#define CONSOLE      PCONSOLE_DEBUG
#define PREFIX       PPREFIX_DEBUG
#define PFUNC(...)   pdebug(__VA_ARGS__)

#include "_main-prints-to-file.h"
