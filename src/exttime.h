/**
 * @file exttime.h
 * @brief Extended time support.
 * @copyright Adequate Systems LLC, 2021-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_TIME_H
#define EXTENDED_TIME_H


#include <time.h>

#ifdef _WIN32
   #include <Windows.h>

#elif ! defined(_POSIX_VERSION) || _POSIX_VERSION < 199309L
   #include <unistd.h>

#endif

/**
 * Suspend the current thread for a specified number of milliseconds.
 * @param ms Number of milliseconds to suspend the current thread
*/
static inline void millisleep(unsigned long ms)
{
#ifdef _WIN32
   Sleep(ms);

/* end Windows */
#elif _POSIX_VERSION >= 199309L
   /* nanosleep() is specified by POSIX.1b-1993... */
   struct timespec ts;

   /* split seconds and nanoseconds from ms parameter */
   ts.tv_sec = (time_t) (ms / 1000UL);
   ts.tv_nsec = (long) ((ms - (ts.tv_sec * 1000UL)) * 1000000L);
   /* perform POSIX compliant sleep */
   /* while(nanosleep(&ts, &ts) != 0); // uninterruptible */
   nanosleep(&ts, &ts);

/* end _POSIX_VERSION >= 199309L */
#else
   usleep(ms * 1000UL);

/* end remaining UNIX-like */
#endif
}

/* end include guard */
#endif
