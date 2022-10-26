/**
 * @private
 * @headerfile extthread.h <extthread.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_THREAD_C
#define EXTENDED_THREAD_C


/* NOTE: For use of pthread_setname_np(), _GNU_SOURCE MUST be defined
 * before ANY includes, and SHALL BE isolated to this compilation unit. */
#define _GNU_SOURCE
#include "extthrd.h"

/* internal support */
#include "exterrno.h"

/* external support */
#if defined(_POSIX_THREADS)
   #include <sys/time.h>

#endif

/* I have my reasons... */
#define boilerplate(cmd)   \
   do { \
      int ecode = cmd; \
      if (ecode) { \
         set_errno(ecode); \
         return -1; \
      } \
      return 0; \
   } while(0)

/**
 * Initialize a Condition Variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_init(Condition *condp)
{
#ifdef _WIN32
   InitializeConditionVariable(condp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cond_init(condp, NULL) );

#endif
}  /* end condition_init() */

/**
 * Signal to a Condition. Unblocks/Wakes at least one thread
 * that is blocked on a condition variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_signal(Condition *condp)
{
#ifdef _WIN32
   WakeConditionVariable(condp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cond_signal(condp) );

#endif
}  /* end condition_signal() */

/**
 * Broadcast to a Condition. Unblocks/Wakes all threads that
 * are blocked on a condition variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_broadcast(Condition *condp)
{
#ifdef _WIN32
   WakeAllConditionVariable(condp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cond_broadcast(condp) );

#endif
}  /* end condition_broadcast() */

/**
 * Wait on a Condition. Blocks/Sleeps the calling thread until
 * a condition variable is signalled or broadcasted to. The
 * calling thread MUST already have a lock on @a mutexp.
 * @param condp Pointer to a ::Condition
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_wait(Condition *condp, Mutex *mutexp)
{
#ifdef _WIN32
   /* on Windows, Mutex is simply a SRWLOCK used only in exclusive mode */
   if (SleepConditionVariableSRW(condp, mutexp, INFINITE, 0)) return 0;
   /* ... an error has occurred */
   set_alterrno(GetLastError());
   return -1;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cond_wait(condp, mutexp) );

#endif
}  /* end condition_wait() */

/**
 * Timed wait on a Condition. Blocks/Sleeps the calling thread
 * until either a condition variable is signalled/broadcasted
 * to or the specified milliseconds have passed. The calling
 * thread MUST already have a lock on @a mutexp.
 * @param condp Pointer to a ::Condition
 * @param mutexp Pointer to a ::Mutex
 * @param ms Milliseconds to wait on condition
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_timedwait(Condition *condp, Mutex *mutexp, unsigned int ms)
{
#ifdef _WIN32
   /* on Windows, Mutex is simply a SRWLOCK used only in exclusive mode */
   if (SleepConditionVariableSRW(condp, mutexp, (DWORD) ms, 0)) return 0;
   /* ... an error has occurred */
   set_alterrno(GetLastError());
   return -1;

#elif defined(_POSIX_THREADS)
   struct timespec abs_timeout;
   struct timeval now;

   /* obtain current time value */
   gettimeofday(&now, NULL);
   /* add ms to time val, store in timeout */
   abs_timeout.tv_sec = now.tv_sec + (ms / 1000UL);
   abs_timeout.tv_nsec =
      ((now.tv_usec + (ms * 1000UL)) * 1000UL) % 1000000000UL;

   boilerplate(
      pthread_cond_timedwait(condp, mutexp, &abs_timeout) );

#endif
}  /* end condition_timedwait() */

/**
 * Destroy a Condition. Free's any memory allocated by the system,
 * and marks Condition as, effectively, uninitialized.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int condition_destroy(Condition *condp)
{
#ifdef _WIN32
   /* Per the Documentation: A condition variable with no waiting
    * threads is in its initial state and can be copied, moved,
    * and forgotten without being explicitly destroyed. */
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cond_destroy(condp) );

#endif
}  /* end condition_destroy() */

/**
 * Initialize a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int mutex_init(Mutex *mutexp)
{
#ifdef _WIN32
   /* NOTE: on Windows, Mutex is a SRWLOCK used only exclusively */
   return rwlock_init((RWLock *) mutexp);

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_mutex_init(mutexp, NULL) );

#endif
}  /* end mutex_init() */

/**
 * Acquire an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int mutex_lock(Mutex *mutexp)
{
#ifdef _WIN32
   /* NOTE: on Windows, Mutex is a SRWLOCK used only exclusively */
   return rwlock_wrlock((RWLock *) mutexp);

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_mutex_lock(mutexp) );

#endif
}  /* end mutex_lock() */

/**
 * Try (non-blocking) acquire an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @param ms Milliseconds to wait for lock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int mutex_trylock(Mutex *mutexp)
{
#ifdef _WIN32
   /* NOTE: on Windows, Mutex is a SRWLOCK used only exclusively */
   return rwlock_trywrlock((RWLock *) mutexp);

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_mutex_trylock(mutexp) );

#endif
}  /* end mutex_trylock() */

/**
 * Release an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int mutex_unlock(Mutex *mutexp)
{
#ifdef _WIN32
   /* NOTE: on Windows, Mutex is a SRWLOCK used only exclusively */
   return rwlock_wrunlock((RWLock *) mutexp);

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_mutex_unlock(mutexp) );

#endif
}  /* end mutex_unlock() */

/**
 * Destroy a Mutex. Free's any memory allocated by the system,
 * and marks Mutex as uninitialized.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int mutex_destroy(Mutex *mutexp)
{
#ifdef _WIN32
   /* NOTE: on Windows, Mutex is a SRWLOCK used only exclusively */
   return rwlock_destroy((RWLock *) mutexp);

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_mutex_unlock(mutexp) );

#endif
}  /* end mutex_destroy() */

/**
 * Initialize a Read Write Lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_init(RWLock *rwlockp)
{
#ifdef _WIN32
   InitializeSRWLock(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_init(rwlockp, NULL) );

#endif
}  /* end rwlock_init() */

/**
 * Lock a ::RWLock object for reading. The calling thread acquires the
 * read lock if no write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_rdlock(RWLock *rwlockp)
{
#ifdef _WIN32
   AcquireSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_rdlock(rwlockp) );

#endif
}  /* end rwlock_rdlock() */

/**
 * Try lock a ::RWLock object for reading. The calling thread acquires the
 * read lock if no write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_tryrdlock(RWLock *rwlockp)
{
#ifdef _WIN32
   TryAcquireSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_tryrdlock(rwlockp) );

#endif
}  /* end rwlock_tryrdlock() */

/**
 * Lock a ::RWLock object for writing. The calling thread acquires the
 * write lock if no read or write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_wrlock(RWLock *rwlockp)
{
#ifdef _WIN32
   AcquireSRWLockExclusive(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_wrlock(rwlockp) );

#endif
}  /* end rwlock_wrlock() */

/**
 * Try lock a ::RWLock object for writing. The calling thread acquires the
 * write lock if no read or write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_trywrlock(RWLock *rwlockp)
{
#ifdef _WIN32
   if (TryAcquireSRWLockExclusive(rwlockp)) return 0;
   /* ... an error has occurred */
   set_alterrno(GetLastError());
   return -1;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_trywrlock(rwlockp) );

#endif
}  /* end rwlock_trywrlock() */

/**
 * Release a ::RWLock object locked for reading.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_rdunlock(RWLock *rwlockp)
{
#ifdef _WIN32
   ReleaseSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_unlock(rwlockp) );

#endif
}  /* end rwlock_rdunlock() */

/**
 * Release a ::RWLock object locked for writing.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_wrunlock(RWLock *rwlockp)
{
#ifdef _WIN32
   ReleaseSRWLockExclusive(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_unlock(rwlockp) );

#endif
}  /* end rwlock_wrunlock() */

/**
 * Destroy a Read Write Lock. Free's any memory allocated by the system.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int rwlock_destroy(RWLock *rwlockp)
{
#ifdef _WIN32
   /* A SRWLock does not need to be explicitly destroyed, so this
    * function simply tries to acquire an exclusive lock on *rwlock
    * and, either immediately releases the lock, or sets errno to the
    * EBUSY code, immitating pthread_rwlock_destory() operation. */
   if (TryAcquireSRWLockExclusive(rwlockp)) {
      ReleaseSRWLockExclusive(rwlockp);
      return 0;
   }

   set_errno(EBUSY);
   return -1;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_rwlock_destroy(rwlockp) );

#endif
}  /* end rwlock_destroy() */

/**
 * Create/Start a new thread. A handle to the created Thread is
 * placed in @a thrdp on success.
 * @param thrdp Pointer to ThreadId to place thread identifier.
 * @param fnp ::ThreadProc function to execute on new thread.
 * @param argp Pointer to arguments to give to @a fnp.
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int thread_create(Thread *thrdp, ThreadRoutine fnp, void *argp)
{
#ifdef _WIN32
   *thrdp = CreateThread(NULL, 0, fnp, argp, 0, NULL);
   if (*thrdp == NULL) {
      set_alterrno(GetLastError());
      return -1;
   }

   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_create(thrdp, NULL, fnp, argp) );

#endif
}  /* end thread_create() */

/**
 * Equality check for Thread handles.
 * @param thrd1 Thread Identifier to check
 * @param thrd2 Thread Identifier to compare against
 * @returns Non-zero value if threads are equal, else 0
*/
int thread_equal(Thread thrd1, Thread thrd2)
{
#ifdef _WIN32
   /* on Windows, Thread's are HANDLE's */
   return CompareObjectHandles(thrd1, thrd2);

#elif defined(_POSIX_THREADS)
   /* pthread Identifiers should be considered opaque */
   return pthread_equal(thrd1, thrd2);

#endif
}  /* end thread_equal() */

/**
 * Join with a thread. Waits for the thread specified by @a thrd to
 * terminate before joining, deallocating any associated resources.
 * @param thrd A ::Thread
 * @returns 0 on success, or non-zero on error. Check errno for details.
*/
int thread_join(Thread thrd)
{
#ifdef _WIN32
   /* wait indefinitely for thread to complete */
   if (WaitForSingleObject(thrd, INFINITE) == WAIT_FAILED) {
      set_alterrno(GetLastError());
      return -1;
   }
   /* close handle to thread */
   CloseHandle(thrd);
   return 0;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_join(thrd, NULL) );

#endif
}  /* end thread_join() */

/**
 * Obtain a Thread Identifier of the current executing thread.
 * @returns (ThreadId) identifier of the current executing thread.
*/
ThreadId thread_selfid(void)
{
#ifdef _WIN32
   return GetCurrentThreadId();

#elif defined(_POSIX_THREADS)
   return pthread_self();

#endif
}  /* end thread_selfid() */

/**
 * Obtain a Thread handle of the current executing thread.
 * @returns (Thread) handle of the current executing thread.
*/
Thread thread_self(void)
{
#ifdef _WIN32
   return GetCurrentThread();

#elif defined(_POSIX_THREADS)
   return pthread_self();

#endif
}  /* end thread_self() */

/**
 * Set the name/description of the the specified by @a thrd.
 * @param thrd Handle of the thread to set name/description
 * @param name String representing the name/description of thread
*/
void thread_setname(Thread thrd, const char *name)
{
#ifdef _WIN32
   wchar_t wname[64];

   mbstowcs(wname, name, 64);
   SetThreadDescription(thrd, wname);

#elif defined(_POSIX_THREADS)
   pthread_setname_np(thrd, name);

#endif
}  /* end thread_setname() */

/**
 * Send a cancellation request to a thread. Does not wait for termination.
 * @param thrd A ::Thread
 * @returns 0 on success, or non-zero on error. Check errno for details.
 * @note Thread cancellation requests are NOT RECOMMENDED under normal
 * conditions, and should instead be reserved for (ungraceful) shutdowns.
*/
int thread_cancel(Thread thrd)
{
#ifdef _WIN32
   if (TerminateThread(thrd, 0)) return 0;
   /* ... an error has occurred */
   set_alterrno(GetLastError());
   return -1;

#elif defined(_POSIX_THREADS)
   boilerplate( pthread_cancel(thrd) );

#endif
}  /* end thread_terminate() */

/* end include guard */
#endif
