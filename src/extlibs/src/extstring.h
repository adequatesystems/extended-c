/**
 * extstring.h - Extended string handling support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 15 September 2021
 * Revised: 25 October 2021
 *
 * NOTES:
 * - "extends" the C Standard Library header file string.h
 *
*/

#ifndef _EXTENDED_STRING_H_
#define _EXTENDED_STRING_H_  /* include guard */


#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Function prototypes for extstring.c */
void memswap(void *ax, void *bx, size_t count);

#ifdef __cplusplus
}
#endif


#endif /* end _EXTENDED_STRING_H_ */
