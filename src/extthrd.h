/**
 * @file extthrd.h
 * @brief Extended thread and mutex support.
 * @details The support functions in this file are based on
 * POSIX Threads. Functionality is extended to Windows systems by
 * wrapping Windows API routines in functions imitating, as much
 * as reasonably possible, associated "pthreads" counterparts.
 * <br/>Thread function syntax: @code
 * ThreadProc threadfn(void *args)
 * {
 *    // ... thread routine ...
 *    Unthread;  // ends the thread
 * }
 * ThreadId startThread(ThreadRoutine func, void *args)
 * {
 *    ThreadId tid;
 *    thread_create(&tid, func, args);
 *    return tid;
 * }
 * int main()
 * {
 *    ThreadId tid = startThread(threadfn, NULL);
 * } @endcode
 * @copyright Adequate Systems LLC, 2021-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
 * @note For use in UNIX-style operating systems, POSIX Threads
 * must be linked by the compiler using the `-pthread` option.
 * 
*/

/* include guard */
#ifndef EXTENDED_THREAD_H
#define EXTENDED_THREAD_H


/* NOTE: For use of pthread_setname_np(), _GNU_SOURCE "must" be defined
 * before "any" includes. But defining _GNU_SOURCE under UNIX systems in
 * the "extos.h" header file may not be a desireable solution.
 * _DEFINE_GNU_SOURCE_IN_EXTOS tells "extos.h" to define _GNU_SOURCE, but
 * only for this compilation unit, reducing side-effects elsewhere. */
#define _DEFINE_GNU_SOURCE_IN_EXTOS
#include "extos.h"

#if OS_WINDOWS
   /* use Windows API threads */

   /* Windows API static initializers */
   #define ext_CONDITION_INITIALIZER   { .semaphore = NULL, .waiting = 0 }
   #define ext_MUTEX_INITIALIZER       NULL
   #define ext_RWLOCK_INITIALIZER      SRWLOCK_INIT

   typedef struct _ext_Condition {
      HANDLE semaphore;
      LONG waiting;
   } ext_Condition;

   /* Windows API datatypes and structs */
   #define ext_Mutex          HANDLE
   #define ext_RWLock         SRWLOCK
   #define ext_ThreadId       DWORD
   #define ext_ThreadProc     DWORD WINAPI
   #define ext_ThreadRoutine  LPTHREAD_START_ROUTINE
   #define ext_ThreadValue    0

/* end OS_WINDOWS */
#elif defined(_POSIX_THREADS)
   /* use pthreads library */
   #include <pthread.h>

   /* POSIX THREAD static initializers */
   #define ext_CONDITION_INITIALIZER   PTHREAD_COND_INITIALIZER
   #define ext_MUTEX_INITIALIZER       PTHREAD_MUTEX_INITIALIZER
   #define ext_RWLOCK_INITIALIZER      PTHREAD_RWLOCK_INITIALIZER

   /* typedef for passing thread process functions as arguments */
   typedef void *(*THREAD_START_ROUTINE)(void *threadArgs);

   /* POSIX Threads datatypes */
   #define ext_Condition      pthread_cond_t
   #define ext_Mutex          pthread_mutex_t
   #define ext_RWLock         pthread_rwlock_t
   #define ext_ThreadId       pthread_t
   #define ext_ThreadProc     void*
   #define ext_ThreadRoutine  THREAD_START_ROUTINE
   #define ext_ThreadValue    NULL

/* end _POSIX_THREADS */
#else
   #error Unable to determine appropriate threading library.

#endif

/**
 * Static Condition initializer.
 * Used to initialize a Condition at the time of declaration.
 * <br/>On Windows, expands to: `CONDITION_VARIABLE_INIT`.
 * <br/>On Unix, expands to: `PTHREAD_COND_INITIALIZER`.
 * <br/>Example usage:
 * @code Condition cond = CONDITION_INITIALIZER; @endcode
*/
#define CONDITION_INITIALIZER ext_CONDITION_INITIALIZER

/**
 * Static Mutex initializer.
 * Used to initialize a Mutex at the time of declaration.
 * <br/>On Windows, expands to: `{ 0 }`.
 * <br/>On Unix, expands to: `PTHREAD_MUTEX_INITIALIZER`.
 * <br/>Example usage:
 * @code Mutex lock = MUTEX_INITIALIZER; @endcode
*/
#define MUTEX_INITIALIZER ext_MUTEX_INITIALIZER

/**
 * Static Read Write Lock initializer.
 * Used to initialize a RWLock at the time of declaration.
 * <br/>On Windows, expands to: `SRWLOCK_INIT`.
 * <br/>On Unix, expands to: `PTHREAD_RWLOCK_INITIALIZER`.
 * <br/>Example usage:
 * @code RWLock lock = RWLOCK_INITIALIZER; @endcode
*/
#define RWLOCK_INITIALIZER ext_RWLOCK_INITIALIZER

/**
 * Thread function datatype. Used to declare a function
 * that may be executed by a thread created with thread_create().
 * <br/>On Windows, expands to: `DWORD WINAPI`.
 * <br/>On Unix, expands to: `void*`.
 * <br/>Example usage: @code
 * ThreadProc threadfn(void *args); @endcode
 * @note `ThreadProc` is to be used exclusively for declaration.
 * To pass a `ThreadProc` function as a parameter to another
 * function, see ::ThreadRoutine.
*/
#define ThreadProc ext_ThreadProc

/**
 * End a `ThreadProc` function. Used in place of a return statement
 * inside a `ThreadProc` function. If return values are necessary,
 * consider utilizing a struct containing a result variable that
 * is passed as an argument.
 * <br/>On Windows, `ext_ThreadValue` expands to: `0`.
 * <br/>On Unix, `ext_ThreadValue` expands to: `NULL`.
 * <br/>Example Usage: @code
 * ThreadProc threadfn(void *args)
 * {
 *    // ... thread routine ...
 *    Unthread;  // ends the thread
 * } @endcode
*/
#define Unthread return ext_ThreadValue

/**
 * Condition variable datatype. Used to define a "meeting place"
 * for multiple threads to "wait" on a certain condition.
 * <br/>On Windows, `ext_Condition` expands to: `CONDITION_VARIABLE`.
 * <br/>On Unix, `ext_Condition` expands to: `pthread_cond_t`.
*/
typedef ext_Condition Condition;

/**
 * Mutually exclusive datatype. Used to define a variable for
 * handling mutually exclusive execution across threads.
 * <br/>On Windows, `ext_Mutex` expands to a self-managed struct.
 * <br/>On Unix, `ext_Mutex` expands to: `pthread_mutex_t`.
*/
typedef ext_Mutex Mutex;

/**
 * Read Write Lock datatype. Used to define a variable for handling
 * shared reads and exclusive writes.
 * <br/>On Windows, expands to: `SRWLOCK`.
 * <br/>On Unix, expands to: `pthread_rwlock_t`.
*/
typedef ext_RWLock RWLock;

/**
 * Thread Identification datatype.
 * Used to handle thread identification used by thread* functions.
 * <br/>On Windows, expands to: `DWORD`.
 * <br/>On Unix, expands to: `pthread_t`.
*/
typedef ext_ThreadId ThreadId;

/**
 * Thread function "parameter" datatype.
 * Used **exclusively** to pass a ::ThreadProc as a function parameter.
 * <br/>Example usage: @code
 * ThreadId startThread(ThreadRoutine func, void *args); @endcode
 * "Wait, but why?"<br/>"Because, Windows..."<br/>"Ah k, say no more."
*/
typedef ext_ThreadRoutine ThreadRoutine;

/* C/C++ compatible function prototypes */
#ifdef __cplusplus
extern "C" {
#endif

int condition_init(Condition *condp);
int condition_signal(Condition *condp);
int condition_broadcast(Condition *condp);
int condition_wait(Condition *condp, Mutex *mutexp);
int condition_timedwait(Condition *condp, Mutex *mutexp, unsigned int ms);
int condition_destroy(Condition *condp);
int mutex_init(Mutex *mutexp);
int mutex_lock(Mutex *mutexp);
int mutex_timedlock(Mutex *mutexp, unsigned int ms);
int mutex_trylock(Mutex *mutexp);
int mutex_unlock(Mutex *mutexp);
int mutex_destroy(Mutex *mutexp);
int rwlock_init(RWLock *rwlockp);
int rwlock_rdlock(RWLock *rwlockp);
int rwlock_tryrdlock(RWLock *rwlockp);
int rwlock_wrlock(RWLock *rwlockp);
int rwlock_trywrlock(RWLock *rwlockp);
int rwlock_rdunlock(RWLock *rwlockp);
int rwlock_wrunlock(RWLock *rwlockp);
int rwlock_destroy(RWLock *rwlockp);
int thread_create(ThreadId *tidp, ThreadRoutine fnp, void *argp);
int thread_equal(ThreadId tid1, ThreadId tid2);
int thread_join(ThreadId tid);
int thread_join_list(ThreadId tidlist[], int count);
ThreadId thread_self(void);
void thread_setname(ThreadId tid, const char *name);
int thread_terminate(ThreadId tid);
int thread_terminate_list(ThreadId tidlist[], int count);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
