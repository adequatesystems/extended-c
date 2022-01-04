/**
 * @file win32lean.h
 * @brief Include Windows "lean and mean" header.
 * @details Ensures `WIN32_LEAN_AND_MEAN` is defined and returned
 * to it's previous state before and after including the
 * <Windows.h> header file, respectively.
 * @copyright © Adequate Systems LLC, 2021-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE
 * @warning This file does not use an "include guard".
 * @warning Intended for use in place of...
 * @code
 * #define WIN32_LEN_AND_MEAN
 * #include <Windows.h>
 * @endcode
*/

#ifndef WIN32_LEAN_AND_MEAN
   #define WIN32_LEAN_AND_MEAN
   #include <Windows.h>
   #undef WIN32_LEAN_AND_MEAN

/* end #ifndef WIN32_LEAN_AND_MEAN */
#else /* WIN32_LEAN_AND_MEAN is predefined */
   #include <Windows.h>

/* end WIN32_LEAN_AND_MEAN */
#endif
