/**
 * @file extprint.h
 * @brief Extended print/logging support.
 * @copyright © Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

#ifndef EXTENDED_PRINT_H
#define EXTENDED_PRINT_H  /* include guard */


#include <stdio.h>

/**
 * @brief No print level. None. Zip.
 * @details Indicates no printing
*/
#define PLEVEL_NONE  0

/**
 * @brief Error logging print level.
 * @details Indicates fatal error level printing
*/
#define PLEVEL_FATAL 1

/**
 * @brief Error logging print level.
 * @details Indicates error level printing
*/
#define PLEVEL_ERROR 2

/**
 * @brief Warning logging print level.
 * @details Indicates warning level printing
*/
#define PLEVEL_WARN  3

/**
 * @brief Standard logging print level.
 * @details Indicates log level printing
*/
#define PLEVEL_LOG   4

/**
 * @brief Debug logging print level.
 * @details Indicates debug level printing
*/
#define PLEVEL_DEBUG 5

/**
 * @brief Print/log a FATAL error message.
 * @param ... arguments you would normally pass to printf()
 * @returns 2, as integer per print_ext()
*/
#define pfatal(...)  \
   print_ext((-1), PLEVEL_FATAL, __LINE__, __FILE__, __VA_ARGS__)

/**
 * @brief Print/log an error message, with description of @a errnum.
 * @param E @a errno associated with error log message
 * @param ... arguments you would normally pass to printf()
 * @returns 1, as integer per print_ext()
*/
#define perrno(E, ...)  \
   print_ext(E, PLEVEL_ERROR, __LINE__, __FILE__, __VA_ARGS__)

/**
 * @brief Print/log an error message.
 * @param ... arguments you would normally pass to printf()
 * @returns 1, as integer per print_ext()
*/
#define perr(...)    \
   print_ext((-1), PLEVEL_ERROR, __LINE__, __FILE__, __VA_ARGS__)

/**
 * @brief Print/log a warning message.
 * @param ... arguments you would normally pass to printf()
 * @returns 0, as integer per print_ext()
*/
#define pwarn(...)   \
   print_ext((-1), PLEVEL_WARN, __LINE__, __FILE__, __VA_ARGS__)

/**
 * @brief Print/log a message.
 * @param ... arguments you would normally pass to printf()
 * @returns 0, as integer per print_ext()
*/
#define plog(...)    \
   print_ext((-1), PLEVEL_LOG, __LINE__, __FILE__, __VA_ARGS__)

/**
 * @brief Print/log a debug message.
 * @param ... arguments you would normally pass to printf()
 * @returns 0, as integer per print_ext()
*/
#define pdebug(...)  \
   print_ext((-1), PLEVEL_DEBUG, __LINE__, __FILE__, __VA_ARGS__)


#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extprint.c */
unsigned get_num_errs(void);
unsigned get_num_logs(void);
int set_output_file(char *fname, char *mode);
void set_output_level(int level);
void set_print_level(int level);
void print(const char *fmt, ...);
int print_ext
(int errnum, int level, int line, const char *file, const char *fmt, ...);
void psticky(const char *fmt, ...);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_PRINT_H */
