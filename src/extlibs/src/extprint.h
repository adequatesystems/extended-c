/**
 * @file extprint.h
 * @brief Extended print/logging support.
 * @copyright © Adequate Systems LLC, 2018-2021. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
*/

#ifndef EXTENDED_PRINT_H
#define EXTENDED_PRINT_H  /* include guard */


#include <stdio.h>


/**
 * @brief No logging print level.
 * @details Disables logging from print functions,
 * when used with set_print_level()
*/
#define PLEVEL_NONE  0

/**
 * @brief Error logging print level.
 * @details Enables logs to @a stderr from perrno(), and perr(),
 * when used with set_print_level()
 * @details Applies settings to perrno() and perr() configuration
 * when used with any other set_print_*() function
*/
#define PLEVEL_ERR   1

/**
 * @brief Standard logging print level.
 * @details Enables logs to @a stderr from perrno(), and perr(),
 * and to @a stdout from plog(), when used with set_print_level()
 * @details Applies settings to plog() configuration when
 * used with any other set_print_*() function
*/
#define PLEVEL_LOG   2

/**
 * @brief Debug logging print level.
 * @details Enables logs to @a stderr from perrno(), and perr()
 * and to @a stdout from plog() and pdebug(), when used with
 * set_print_level()
 * @details Applies settings to pdebug() configuration when
 * used with any other set_print_*() function
*/
#define PLEVEL_DEBUG 3


#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extprint.c */
void psticky(char *msg);
void print(const char *fmt, ...);
int perrno(int errnum, const char *fmt, ...);
int perr(const char *fmt, ...);
int plog(const char *fmt, ...);
int pdebug(const char *fmt, ...);
unsigned get_perr_counter(void);
unsigned get_plog_counter(void);
unsigned get_pdebug_counter(void);
void set_perr_fp(FILE *fp);
void set_plog_fp(FILE *fp);
void set_pdebug_fp(FILE *fp);
void set_perr_prefix(char *prefix);
void set_plog_prefix(char *prefix);
void set_pdebug_prefix(char *prefix);
void set_print_level(char level);
void set_print_timestamp(char time);

#ifdef __cplusplus
}
#endif


#endif  /* end EXTENDED_PRINT_H */
