
#include "../extio.h"

#define FP           Pstdoutfp
#define FNAME        "stdout.log"
#define CONSOLE      PCONSOLE_NUL
#define PREFIX       PPREFIX_LOG
#define PFUNC(...)   plog(__VA_ARGS__)

#include "_main-prints-to-file.h"
