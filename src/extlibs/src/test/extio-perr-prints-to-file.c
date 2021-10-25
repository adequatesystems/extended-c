
#include "../extio.h"

#define FP           Pstderrfp
#define FNAME        "stderr.log"
#define CONSOLE      PCONSOLE_NUL
#define PREFIX       PPREFIX_ERR
#define PFUNC(...)   perr(__VA_ARGS__)

#include "_main-prints-to-file.h"
