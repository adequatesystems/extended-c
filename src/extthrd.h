/**
 * @file extthrd.h
 * @brief Extended thread and mutex support.
 * @details The support functions in this file are based on POSIX Threads.
 * On Windows systems, functions wrap the Windows API imitating, as much
 * as reasonably possible, the POSIX Thread functions they are based on.
 * On Non-Windows systems, functions directly call the pthreads library,
 * which must be linked by the compiler/linker using the `-pthread` option.
 *
 * Thread function syntax: @code
 * ThreadProc threadfn(void *args)
 * {
 *    // ... thread routine ...
 *    Unthread;  // ends the thread
 * }
 * int startThread(Thread *thrdp, ThreadRoutine func, void *args)
 * {
 *    // NOTE: this function simply demonstrates the requirement to use
 *    // ThreadRoutine as a parameter type when passing ThreadProc args
 *    return thread_create(thrdp, func, args);
 * }
 * int main()
 * {
 *    Thread thread;
 *
 *    if (startThread(&thread, threadfn, NULL) != 0) {
 *       // check errno, handle error
 *    } else {
 *       // Thread handle is placed in thread
 *       thread_join(thread); // blocks until thread has finished
 *    }
 * } @endcode
 * @copyright Adequate Systems LLC, 2021-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_THREAD_H
#define EXTENDED_THREAD_H


#ifdef _WIN32
   /* use Windows API threads */
   #include <win32lean.h>

   /* Windows API static initializers */
   #define ext_CONDITION_INITIALIZER   CONDITION_VARIABLE_INIT
   #define ext_MUTEX_INITIALIZER       SRWLOCK_INIT
   #define ext_RWLOCK_INITIALIZER      SRWLOCK_INIT

   /* Windows API datatypes and structs */
   #define ext_Condition      CONDITION_VARIABLE
   #define ext_Mutex          SRWLOCK
   #define ext_RWLock         SRWLOCK
   #define ext_Thread         HANDLE
   #define ext_ThreadId       DWORD
   #define ext_ThreadProc     DWORD WINAPI
   #define ext_ThreadRoutine  LPTHREAD_START_ROUTINE
   #define ext_ThreadReturn   0

/* end OS_WINDOWS */
#else
   /* use pthreads library */
   #include <pthread.h>
   #include <unistd.h>

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
   #define ext_Thread         pthread_t
   #define ext_ThreadId       pthread_t
   #define ext_ThreadProc     void*
   #define ext_ThreadRoutine  THREAD_START_ROUTINE
   #define ext_ThreadReturn   NULL

/* end UNIX-like */
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
 * <br/>On Windows, `ext_ThreadReturn expands to: `0`.
 * <br/>On Unix, `ext_ThreadReturn expands to: `NULL`.
 * <br/>Example Usage: @code
 * ThreadProc threadfn(void *args)
 * {
 *    // ... thread routine ...
 *    Unthread;  // ends the thread
 * } @endcode
*/
#define Unthread return ext_ThreadReturn

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
 * <br/>On Windows, `ext_Mutex` expands to: `SRWLOCK` (exclusive use only).
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
 * Thread handle datatype.
 * Used to reference a specific thread used by thread* functions.
 * <br/>On Windows, expands to: `HANDLE`.
 * <br/>On Unix, expands to: `pthread_t`.
*/
typedef ext_Thread Thread;

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
int thread_create(Thread *thrdp, ThreadRoutine fnp, void *argp);
int thread_equal(Thread thrd1, Thread thrd2);
int thread_join(Thread thrd);
ThreadId thread_selfid(void);
Thread thread_self(void);
void thread_setname(Thread thrd, const char *name);
int thread_cancel(Thread thrd);

#ifdef __cplusplus
}  /* end extern "C" */
#endif

/* end include guard */
#endif
