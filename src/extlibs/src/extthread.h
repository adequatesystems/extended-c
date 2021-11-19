/**
 * extthread.h - Extended Thread and Mutex support header
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 29 October 2021
 *
 * The support functions in this file are based on the pthreads LIB
 * (POSIX Threads). Functionality is extended to Windows systems by
 * wrapping Windows API routines in functions imitating, as much as
 * reasonably possible, their associated pthreads counterparts.
 *
 * NOTES:
 * - A Mutex can be statically initialized using MUTEX_INITIALIZER,
 *   RWLock can be statically initialized using RWLOCK_INITIALIZER.
 * - A function designed to run in a new thread SHALL be of format:
 *    Threaded thread_functionname(void *args)
 *    {
 *       // ... multithreaded routine ...
 *       Unthread;  // ends the thread
 *    }
 *
*/

#ifndef _EXTENDED_THREAD_H_
#define _EXTENDED_THREAD_H_  /* include guard */


#ifndef _WIN32  /* assume POSIX */

#include <pthread.h>  /* must compile and link with -pthread */

/* POSIX THREAD static initializers */
#define MUTEX_INITIALIZER   PTHREAD_MUTEX_INITIALIZER
#define RWLOCK_INITIALIZER  PTHREAD_RWLOCK_INITIALIZER

/* POSIX THREAD datatypes and structs */
#define Unthread  return NULL       /* end a Threaded function */
#define Threaded  void*             /* thread function datatype */
#define ThreadId  pthread_t         /* thread identification datatype */
#define Mutex     pthread_mutex_t   /* mutually exclusive lock */
#define RWLock    pthread_rwlock_t  /* shared read / exclusive write lock */

/* POSIX THREAD function redefinitions... */
   /* ... threading functions, return 0 on success else error code. */
#define thread_start(PTID, PFUNC, PARG) \
                              pthread_create(PTID, NULL, PFUNC, PARG)
#define thread_wait(TID)      pthread_join(TID, NULL) /* BLOCKING */
   /* ... mutex lock functions, return 0 on success else error code. */
#define mutex_init(PM)        pthread_mutex_init(PM, NULL)
#define mutex_lock(PM)        pthread_mutex_lock(PM)      /* BLOCKING */
#define mutex_unlock(PM)      pthread_mutex_unlock(PM)
#define mutex_free(PM)        pthread_mutex_destroy(PM)
   /* ... read write lock functions, return 0 on success else error code. */
#define rwlock_init(PRWL)     pthread_rwlock_init(PRWL,NULL)
#define rwlock_rdlock(PRWL)   pthread_rwlock_rdlock(PRWL) /* BLOCKING */
#define rwlock_wrlock(PRWL)   pthread_rwlock_wrlock(PRWL) /* BLOCKING */
#define rwlock_rdunlock(PRWL) pthread_rwlock_unlock(PRWL)
#define rwlock_wrunlock(PRWL) pthread_rwlock_unlock(PRWL)
#define rwlock_free(PRWL)     pthread_rwlock_destroy(PRWL)

#ifdef __cplusplus
extern "C" {
#endif

#else  /* end POSIX, assume WINDOWS */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/* Windows API static initializers */
#define MUTEX_INITIALIZER   { .lock = { 0 }, .init = (-1) }
#define RWLOCK_INITIALIZER  SRWLOCK_INIT

/* Windows API datatypes and structs */
#define Unthread  return 0       /* end a Threaded function */
#define Threaded  DWORD WINAPI   /* thread function datatype */
#define ThreadId  DWORD          /* thread identification datatype */
#define RWLock    SRWLOCK        /* shared read / exclusive write lock */

/* A Mutually exclusive lock datatype, utilizing Windows' CRITICAL_SECTION
 * to more closely imitate pthread's pthread_mutex_t element. Since there
 * is no static initialization method for a CRITICAL_SECTION, the struct
 * also holds an initialization variable to indicate the initialization
 * status of the CRITICAL_SECTION. If static initialization is chosen,
 * actual initialization may occur in the first call to mutex_lock(). */
typedef struct _Mutex {
   CRITICAL_SECTION lock;
   volatile char init;
} Mutex;

#ifdef __cplusplus
extern "C" {
#endif

/* Windows function prototypes for extthread.c */
int thread_create(ThreadId *tid, Threaded func, void *arg);
int thread_wait(ThreadId tid);
int mutex_init(Mutex *mutex);
int mutex_lock(Mutex *mutex);
int mutex_unlock(Mutex *mutex);
int mutex_free(Mutex *mutex);
int rwlock_init(RWLock *rwlock);
int rwlock_rdlock(RWLock *rwlock);
int rwlock_wrlock(RWLock *rwlock);
int rwlock_rdunlock(RWLock *rwlock);
int rwlock_wrunlock(RWLock *rwlock);
int rwlock_free(RWLock *rwlock);

#endif  /* end Windows */

/* Function prototypes for extthread.c */
int thread_multiwait(int count, ThreadId tidlist[]);

#ifdef __cplusplus
}
#endif


#endif  /* end _EXTENDED_THREAD_H_ */
