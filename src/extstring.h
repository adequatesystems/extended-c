/**
 * @file extstring.h
 * @brief Extended string/memory handling support.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_STRING_H
#define EXTENDED_STRING_H


#include <string.h>

/* C/C++ compatible function prototypes for extthread.c */
#ifdef __cplusplus
extern "C" {
#endif

void memswap(void *ax, void *bx, size_t count);

/* end extern "C" {} for C++ */
#ifdef __cplusplus
}
#endif

/* end include guard */
#endif
