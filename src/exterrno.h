/**
 * @file exterrno.h
 * @brief Extended error number support.
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_ERRNO_C
#define EXTENDED_ERRNO_C


#include <errno.h>

/**
 * Set errno to the integer value of @a e.
 * @param e Value to set errno
*/
#ifdef _WIN32
   #define set_errno(e) _set_errno(e)

#else
   #define set_errno(e) ( errno = e )

#endif

/**
 * Set errno to an alternate error system, like the one used by Windows.
 * Makes the error code value negative before setting errno, to avoid
 * conflict with the Standard C error number system. When combined with
 * the strerror_ext() function, negative errno values are converted to
 * their original positive values and interpretted accordingly.
 * @param e Alternate system error number value to set
*/
#define set_alterrno(e) set_errno(-(e))

/* end include guard */
#endif
