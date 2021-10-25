
#include "../extio.h"

#define FP           stderr
#define FNAME        "stderr.log"
#define CONSOLE      PCONSOLE_ERR
#define PREFIX       PPREFIX_ERR
#define PFUNC(...)   perrno(1, __VA_ARGS__)

#include "_main-prints-to-file.h"
