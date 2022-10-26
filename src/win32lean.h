/**
 * @private
 * @file win32lean.h
 * @brief Windows System "lean and mean" include (INTERNAL USE).
 * @details Includes the `<Windows.h>` system header file ensuring
 * `WIN32_LEAN_AND_MEAN` is defined and returned to its original state,
 * before and after the include, respectively.
 * @copyright Adequate Systems LLC, 2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_WIN32LEAN_H
#define EXTENDED_WIN32LEAN_H


#ifdef WIN32_LEAN_AND_MEAN
   #define RESET_WIN32_LEAN_AND_MEAN
#endif

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#ifdef RESET_WIN32_LEAN_AND_MEAN
   #undef RESET_WIN32_LEAN_AND_MEAN
   /* return to previous state (undefined) */
   #undef WIN32_LEAN_AND_MEAN
#endif

/* end include guard */
#endif
