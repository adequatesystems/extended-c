/**
 * @private
 * @headerfile extthread.h <extthread.h>
 * @copyright Adequate Systems LLC, 2018-2022. All Rights Reserved.
 * <br />For license information, please refer to ../LICENSE.md
*/

/* include guard */
#ifndef EXTENDED_THREAD_C
#define EXTENDED_THREAD_C


#include "extthrd.h"
#if OS_UNIX
   #include <sys/time.h>

#endif

/**
 * Initialize a Condition Variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, else error number.
*/
int condition_init(Condition *condp)
{
#if OS_WINDOWS
   /* create Semaphore HANDLE and zero waiting counter */
   condp->semaphore = CreateSemaphore(NULL, 0L, 0x7fffffffL, NULL);
   if (condp->semaphore == NULL) return GetLastError();
   condp->waiting = 0;
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_cond_init(condp, NULL);

#endif
}

#if OS_WINDOWS

/**
 * @private
 * Initialize a "statically initialized" Condition Variable. WINDOWS ONLY.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, else error number.
*/
static int condition_init_static(Condition *condp)
{
   /* create Sempahore HANDLE and perform interlocked exchange */
   HANDLE semaphore = CreateMutex(NULL, FALSE, NULL);
   if (semaphore == NULL) return GetLastError();
   /* condp->waiting is zero'd by static initialization declaration */
   if (InterlockedCompareExchangePointer(
         (PVOID *) &(condp->semaphore), (PVOID) semaphore, NULL) != NULL) {
      /* close HANDLE if semaphore is non-NULL */
      CloseHandle(semaphore);
      return EBUSY;
   } else return 0;
}

#endif

/**
 * Signal to a Condition. Unblocks/Wakes at least one thread
 * that is blocked on a condition variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, else error number.
*/
int condition_signal(Condition *condp)
{
#if OS_WINDOWS
   /* (atomically) obtain the value of waiting */
   LONG waiting = InterlockedIncrement(&(condp->waiting)) - 1;
   InterlockedDecrement(&(condp->waiting));
   /* ensure something is waiting on condition */
   if (waiting < 1) return 0;
   /* release semaphore by one (1) "signal" */
   return ReleaseSemaphore(condp->semaphore, 1, NULL) ? 0 : GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_cond_signal(condp);

#endif
}

/**
 * Broadcast to a Condition. Unblocks/Wakes all threads that
 * are blocked on a condition variable.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, else error number.
*/
int condition_broadcast(Condition *condp)
{
#if OS_WINDOWS
   /* release semaphore by the amount currently "waiting" */
   int ecode = ReleaseSemaphore(condp->semaphore,
      InterlockedIncrement(&(condp->waiting)) - 1, NULL);
   InterlockedDecrement(&(condp->waiting));
   return ecode ? 0 : GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_cond_broadcast(condp);

#endif
}

/**
 * Wait on a Condition. Blocks/Sleeps the calling thread until
 * a condition variable is signalled or broadcasted to. The
 * calling thread MUST already have a lock on @a mutexp.
 * @param condp Pointer to a ::Condition
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
*/
int condition_wait(Condition *condp, Mutex *mutexp)
{
#if OS_WINDOWS
   int ecode;

   /* release lock on Mutex */
   if (!ReleaseMutex(*mutexp)) return GetLastError();
   /* initialize semaphore if declared "statically initialized" */
   if (condp->semaphore == NULL) {
      ecode = condition_init_static(condp);
      if (ecode != EBUSY) return ecode;
   }
   /* prepare HANDLE array of "multiple objects" */
   HANDLE objects[2] = { condp->semaphore, *mutexp };
   /* wait on Condition AND Mutex -- adjust waiting counter */
   InterlockedIncrement(&(condp->waiting));
   ecode = WaitForMultipleObjects(2, objects, TRUE, INFINITE);
   InterlockedDecrement(&(condp->waiting));
   /* check result of WaitForMultipleObjects */
   switch (ecode) {
      case WAIT_FAILED: return GetLastError();
      case WAIT_OBJECT_0: /* fallthrough */
      case (WAIT_OBJECT_0 + 1): return 0;
      default: return EINVAL;
   }

#elif defined(_POSIX_THREADS)
   return pthread_cond_wait(condp, mutexp);

#endif
}

/**
 * Timed wait on a Condition. Blocks/Sleeps the calling thread
 * until either a condition variable is signalled/broadcasted
 * to or the specified milliseconds have passed. The calling
 * thread MUST already have a lock on @a mutexp.
 * @param condp Pointer to a ::Condition
 * @param mutexp Pointer to a ::Mutex
 * @param ms Milliseconds to wait on condition
 * @returns 0 on success, else error number.
*/
int condition_timedwait(Condition *condp, Mutex *mutexp, unsigned int ms)
{
#if OS_WINDOWS
   int ecode;

   /* release lock on Mutex */
   if (!ReleaseMutex(*mutexp)) return GetLastError();
   /* initialize semaphore if declared "statically initialized" */
   if (condp->semaphore == NULL) {
      ecode = condition_init_static(condp);
      if (ecode != EBUSY) return ecode;
   }
   /* prepare HANDLE array of "multiple objects" */
   HANDLE objects[2] = { condp->semaphore, *mutexp };
   /* wait (timed) on Condition AND Mutex -- adjust waiting counter */
   InterlockedIncrement(&(condp->waiting));
   ecode = WaitForMultipleObjects(2, objects, TRUE, ms);
   InterlockedDecrement(&(condp->waiting));
   /* check result of WaitForMultipleObjects */
   switch (ecode) {
      case WAIT_FAILED: return GetLastError();
      case WAIT_TIMEOUT: return ETIMEDOUT;
      case WAIT_OBJECT_0: /* fallthrough */
      case (WAIT_OBJECT_0 + 1): return 0;
      default: return EINVAL;
   }

#elif defined(_POSIX_THREADS)
   struct timespec abs_timeout;
   struct timeval now;

   /* obtain current time value */
   gettimeofday(&now, NULL);
   /* add ms to time val, store in timeout */
   abs_timeout.tv_sec = now.tv_sec + (ms / 1000UL);
   abs_timeout.tv_nsec =
      ((now.tv_usec + (ms * 1000UL)) * 1000UL) % 1000000000UL;

   return pthread_cond_timedwait(condp, mutexp, &abs_timeout);

#endif
}

/**
 * Destroy a Condition. Free's any memory allocated by the system,
 * and marks Condition as, effectively, uninitialized.
 * @param condp Pointer to a ::Condition
 * @returns 0 on success, else error number.
*/
int condition_destroy(Condition *condp)
{
#if OS_WINDOWS
   /* close handle to semaphore */
   if (CloseHandle(condp->semaphore)) {
      condp->semaphore = NULL;
      return 0;
   }
   return GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_cond_destroy(condp);

#endif
}

/**
 * Initialize a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
*/
int mutex_init(Mutex *mutexp)
{
#if OS_WINDOWS
   /* create Mutex HANDLE */
   *mutexp = CreateMutex(NULL, FALSE, NULL);
   if (*mutexp == NULL) return GetLastError();
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_mutex_init(mutexp, NULL);

#endif
}

#if OS_WINDOWS

/**
 * @private
 * Initialize a "statically initialized" Mutex. WINDOWS ONLY.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
*/
static int mutex_init_static(Mutex *mutexp)
{
   /* create Mutex HANDLE and perform interlocked exchange */
   HANDLE mutex = CreateMutex(NULL, FALSE, NULL);
   if (mutex == NULL) return GetLastError();
   if (InterlockedCompareExchangePointer(
         (PVOID *) mutexp, (PVOID) mutex, NULL) != NULL) {
      /* close HANDLE if mutex is non-NULL */
      CloseHandle(mutex);
      return EBUSY;
   } else return 0;
}

#endif

/**
 * Acquire an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
 * @note On Windows, a statically initialized Mutex will be
 * initialized on the first call to mutex_lock().
*/
int mutex_lock(Mutex *mutexp)
{
#if OS_WINDOWS
   /* initialize mutex if statically initialized */
   if (*mutexp == NULL) {
      int ecode = mutex_init_static(mutexp);
      if (ecode != EBUSY) return ecode;
   }
   /* wait for exclusive lock -- no timeout */
   switch (WaitForSingleObject(*mutexp, INFINITE)) {
      case WAIT_FAILED: return GetLastError();
      case WAIT_OBJECT_0: return 0;
      default: return EINVAL;
   }

#elif defined(_POSIX_THREADS)
   return pthread_mutex_lock(mutexp);

#endif
}

/**
 * Acquire an exclusive lock on a Mutex, or timeout.
 * @param mutexp Pointer to a ::Mutex
 * @param ms Milliseconds to wait for lock
 * @returns 0 on success, else error number.
 * @note On Windows, a statically initialized Mutex will be
 * initialized on the first call to mutex_timedlock().
*/
int mutex_timedlock(Mutex *mutexp, unsigned int ms)
{
#if OS_WINDOWS
   /* initialize mutex if statically initialized */
   if (*mutexp == NULL) mutex_init(mutexp);
   /* wait (ms) for exclusive lock, or timeout */
   switch (WaitForSingleObject(*mutexp, ms)) {
      case WAIT_FAILED: return GetLastError();
      case WAIT_TIMEOUT: return ETIMEDOUT;
      case WAIT_OBJECT_0: return 0;
      default: return EINVAL;
   }

#elif defined(_POSIX_THREADS)
   struct timespec abs_timeout;
   struct timeval now;

   /* obtain current time value */
   gettimeofday(&now, NULL);
   /* add ms to time val, store in timeout */
   abs_timeout.tv_sec = now.tv_sec + (ms / 1000UL);
   abs_timeout.tv_nsec =
      ((now.tv_usec + (ms * 1000UL)) * 1000UL) % 1000000000UL;

   return pthread_mutex_timedlock(mutexp, &abs_timeout);

#endif
}

/**
 * Try (non-blocking) acquire an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @param ms Milliseconds to wait for lock
 * @returns 0 on success, else error number.
 * @note On Windows, a statically initialized Mutex will be
 * initialized on the first call to mutex_timedlock().
*/
int mutex_trylock(Mutex *mutexp)
{
#if OS_WINDOWS
   /* initialize mutex if statically initialized */
   if (*mutexp == NULL) mutex_init(mutexp);
   /* wait (ms) for exclusive lock, or timeout */
   switch (WaitForSingleObject(*mutexp, 0)) {
      case WAIT_FAILED: return GetLastError();
      case WAIT_TIMEOUT: return EBUSY;
      case WAIT_OBJECT_0: return 0;
      default: return EINVAL;
   }

#elif defined(_POSIX_THREADS)
   return pthread_mutex_trylock(mutexp);

#endif
}

/**
 * Release an exclusive lock on a Mutex.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
*/
int mutex_unlock(Mutex *mutexp)
{
#if OS_WINDOWS
   /* release lock on mutex */
   return ReleaseMutex(*mutexp) ? 0 : GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_mutex_unlock(mutexp);

#endif
}

/**
 * Destroy a Mutex. Free's any memory allocated by the system,
 * and marks Mutex as uninitialized.
 * @param mutexp Pointer to a ::Mutex
 * @returns 0 on success, else error number.
*/
int mutex_destroy(Mutex *mutexp)
{
#if OS_WINDOWS
   /* close handle to mutex */
   if (CloseHandle(*mutexp)) {
      *mutexp = NULL;
      return 0;
   }
   return GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_mutex_unlock(mutexp);

#endif
}

/**
 * Initialize a Read Write Lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error code.
*/
int rwlock_init(RWLock *rwlockp)
{
#if OS_WINDOWS
   InitializeSRWLock(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_init(rwlockp, NULL);

#endif
}

/**
 * Lock a ::RWLock object for reading. The calling thread acquires the
 * read lock if no write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error code.
 * @note This function can block the execution of a thread.
*/
int rwlock_rdlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   AcquireSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_rdlock(rwlockp);

#endif
}

/**
 * Try lock a ::RWLock object for reading. The calling thread acquires the
 * read lock if no write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, EBUSY if unable to acquire lock immediately,
 * else error code.
*/
int rwlock_tryrdlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   TryAcquireSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_tryrdlock(rwlockp);

#endif
}

/**
 * Lock a ::RWLock object for writing. The calling thread acquires the
 * write lock if no read or write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error code.
 * @note This function can block the execution of a thread.
*/
int rwlock_wrlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   AcquireSRWLockExclusive(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_wrlock(rwlockp);

#endif
}

/**
 * Try lock a ::RWLock object for writing. The calling thread acquires the
 * write lock if no read or write locks are held or waiting on the lock.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, EBUSY if unable to acquire lock immediately,
 * else error code.
*/
int rwlock_trywrlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   return TryAcquireSRWLockExclusive(rwlockp) ? 0 : GetLastError();

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_trywrlock(rwlockp);

#endif
}

/**
 * Release a ::RWLock object locked for reading.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error code.
*/
int rwlock_rdunlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   ReleaseSRWLockShared(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_unlock(rwlockp);

#endif
}

/**
 * Release a ::RWLock object locked for writing.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error code.
*/
int rwlock_wrunlock(RWLock *rwlockp)
{
#if OS_WINDOWS
   ReleaseSRWLockExclusive(rwlockp);
   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_unlock(rwlockp);

#endif
}

/**
 * Destroy a Read Write Lock. Free's any memory allocated by the system.
 * @param rwlockp Pointer to a ::RWLock
 * @returns 0 on success, else error number.
*/
int rwlock_destroy(RWLock *rwlockp)
{
#if OS_WINDOWS
   /* A SRWLock does not need to be explicitly destroyed, so this
    * function simply tries to acquire an exclusive lock on *rwlock
    * and, either immediately releases the lock, or return the
    * EBUSY code, immitating pthread_rwlock_destory() operation. */
   if (TryAcquireSRWLockExclusive(rwlockp)) {
      ReleaseSRWLockExclusive(rwlockp);
      return 0;
   }

   return EBUSY;

#elif defined(_POSIX_THREADS)
   return pthread_rwlock_destroy(rwlockp);

#endif
}

/**
 * Create/Start a new thread. Thread identifier is placed in @a tidp.
 * @param tidp Pointer to ThreadId to place thread identifier.
 * @param fnp ::ThreadProc function to execute on new thread.
 * @param argp Pointer to arguments to give to @a fnp.
 * @returns 0 on success, else error number
*/
int thread_create(ThreadId *tidp, ThreadRoutine fnp, void *argp)
{
#if OS_WINDOWS
   HANDLE thandle = CreateThread(NULL, 0, fnp, argp, 0, tidp);
   if (thandle == NULL) return GetLastError();
   CloseHandle(thandle);

   return 0;

#elif defined(_POSIX_THREADS)
   return pthread_create(tidp, NULL, fnp, argp);

#endif
}  /* end thread_create() */

/**
 * Equality check for ThreadId.
 * @param tid1 Thread Identifier to check
 * @param tid2 Thread Identifier to compare against
 * @returns Non-zero value if threads are equal, else 0
*/
int thread_equal(ThreadId tid1, ThreadId tid2)
{
#if OS_WINDOWS
   /* Windows Thread Identifiers are simply DWORD's */
   return (tid1 == tid2);

#elif defined(_POSIX_THREADS)
   /* pthread Identifiers should be considered opaque */
   return pthread_equal(tid1, tid2);

#endif
}  /* end thread_equal() */

/**
 * Join with a terminated thread.
 * Waits for the thread specified by @a tid to terminate.
 * @param tid A ::ThreadId
 * @returns 0 on success, else error number.
*/
int thread_join(ThreadId tid)
{
#if OS_WINDOWS
   HANDLE thandle;
   int ecode = 0;

   /* acquire Thread HANDLE from thread id */
   thandle = OpenThread(SYNCHRONIZE, FALSE, tid);
   if(thandle == NULL) return GetLastError();
   /* wait indefinitely for thread to complete */
   if(WaitForSingleObject(thandle, INFINITE) == WAIT_FAILED) {
      ecode = GetLastError();
   }
   /* close handle to thread */
   CloseHandle(thandle);
   return ecode;

#elif defined(_POSIX_THREADS)
   return pthread_join(tid, NULL);

#endif
}  /* end thread_join() */

/**
 * Join with a list of terminated threads.
 * Waits for all threads in a list specified by tidlist[count] to
 * terminate and "joins" with them in a sequential manner.
 * @param tidlist Pointer to a list of ::ThreadId's
 * @param count Number of threads in list
 * @returns 0 on success, else the first error which occurred.
 * @note If a ::ThreadId within the @a tidlist evaluates as Zero (0),
 * that ::ThreadId will be skipped.
*/
int thread_join_list(ThreadId tidlist[], int count)
{
   int i, temp, ecode;

   for(ecode = temp = i = 0; i < count; i++) {
      if (tidlist[i]) temp = thread_join(tidlist[i]);
      if (temp && !ecode) ecode = temp;
   }

   return ecode;
}

ThreadId thread_self(void)
{
#if OS_WINDOWS
   return GetCurrentThreadId();

#elif defined(_POSIX_THREADS)
   return pthread_self();

#endif
}

void thread_setname(ThreadId tid, const char *name)
{
#if OS_WINDOWS
   wchar_t wname[64];

   mbstowcs(wname, name, 64);
   SetThreadDescription(GetCurrentThread(), wname);

#elif defined(_POSIX_THREADS)
   pthread_setname_np(tid, name);

#endif
}

/**
 * Send a termination request to a thread. Does not wait for termination.
 * @param tid A ::ThreadId
 * @returns 0 on success, else error number.
 * @note It is not recommended to forcefully terminate threads in running
 * processes, and should instead be reserved for (ungraceful) shutdowns.
*/
int thread_terminate(ThreadId tid)
{
#if OS_WINDOWS
   HANDLE thandle;
   int ecode;

   /* acquire Thread HANDLE from thread id */
   thandle = OpenThread(SYNCHRONIZE, FALSE, tid);
   if(thandle == NULL) return GetLastError();
   /* wait indefinitely for thread to complete */
   ecode = TerminateThread(thandle, 0) ? 0 : GetLastError();
   /* close handle to thread */
   CloseHandle(thandle);
   return ecode;

#elif defined(_POSIX_THREADS)
   return pthread_cancel(tid);

#endif
}  /* end thread_terminate() */

/**
 * Terminate a list of threads. Sends a termination request to all threads
 * in a list specified by @a tidlist[count]. Does not wait for termination.
 * @param tidlist Pointer to a list of ::ThreadId's
 * @param count Number of threads in list
 * @returns 0 on success, else the first error which occurred.
 * @note If a ::ThreadId within the @a tidlist evaluates as Zero (0),
 * that ::ThreadId will be skipped.
*/
int thread_terminate_list(ThreadId tidlist[], int count)
{
   int i, temp, ecode;

   for(ecode = temp = i = 0; i < count; i++) {
      if (tidlist[i]) temp = thread_terminate(tidlist[i]);
      if (temp && !ecode) ecode = temp;
   }

   return ecode;
}  /* end thread_terminate_list() */

/* end include guard */
#endif
