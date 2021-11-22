/**
 * extthread.c - Extended Thread and Mutex support
 *
 * Copyright (c) 2021 Adequate Systems, LLC. All Rights Reserved.
 * For more information, please refer to ../LICENSE
 *
 * Date: 29 October 2021
 *
*/

#ifndef _EXTENDED_THREAD_C_
#define _EXTENDED_THREAD_C_  /* include guard */


#include "extthread.h"

#ifdef _WIN32  /* assume Windows */

/* Create a new thread on Windows and store it's thread identifier.
 * Return 0 on success, else GetLastError(). */
int thread_create(ThreadId *tid, Threaded func, void *arg)
{
   HANDLE thandle;

   /* create thread and check returned handle for errors */
   thandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, tid);
   if(thandle == NULL) return GetLastError();

   return 0;
}

/* Wait for a thread on Windows to complete. (BLOCKING)
 * Returns 0 on success, else GetLastError(). */
int thread_wait(ThreadId tid)
{
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
}

/* Initialize a Mutex on Windows and set initialized.
 * Always returns 0 on Windows. */
int mutex_init(Mutex *mutex)
{
   /* CRITICAL_SECTION initialization is guarded by an initialization
    * spinlock, which is 32-bit aligned as required by the Windows
    * API's InterlockedCompareExchange() atomic function. */
   __declspec(align(4)) static volatile LONG spinlock = 0;

   /* per pthreads_mutex_init(), return EBUSY for initialized mutex */
   if (mutex->init > 0) return EBUSY;
   /* acquire exclusive spin lock for initialisation */
   while(InterlockedCompareExchange(&spinlock, 1, 0));
   /* vv SPINLOCKED SECTION START */

   /* attempt initialization if uninitialised */
   if(mutex->init < 1) {
      /* initialize critical section inside Mutex */
      InitializeCriticalSection(&mutex->lock);
      /* set Mutex initialized */
      mutex->init = 1;
   }

   /* ^^ END SPINLOCK SECTION */
   /* release exclusive spin lock */
   spinlock = 0;

   return 0;
}

/* Acquire an exclusive lock on Windows.
 * Returns 0 on success, or EINVAL if mutex is not initialized. */
int mutex_lock(Mutex *mutex)
{
   /* per pthreads_mutex_init(), return EINVAL for uninitialized mutex */
   if (!mutex->init) return EINVAL;
   /* initialize mutex if statically initialized */
   if (mutex->init < 0) mutex_init(mutex);
   /* acquire exclusive critical section lock */
   EnterCriticalSection(&mutex->lock);

   return 0;
}

/* Release an exclusive lock on Windows.
 * Always returns 0 on Windows. */
int mutex_unlock(Mutex *mutex)
{
   /* release exclusive critical section lock */
   LeaveCriticalSection(&mutex->lock);

   return 0;
}

/* Uninitalize a Mutex on Windows.
 * Always returns 0 on Windows. */
int mutex_free(Mutex *mutex)
{
   /* destroy critical section */
   DeleteCriticalSection(&mutex->lock);
   /* set Mutex uninitialized */
   mutex->init = 0;

   return 0;
}

/* Read write lock (RWLock) functions on Windows.
 * Always returns 0 on Windows, except rwlock_free(),
 * which returns EBUSY when a rwlock is currently in place. */
int rwlock_init(RWLock *rwlock)
{
   InitializeSRWLock(rwlock); return 0;
}

int rwlock_rdlock(RWLock *rwlock)
{
   AcquireSRWLockShared(rwlock); return 0;
}

int rwlock_wrlock(RWLock *rwlock)
{
   AcquireSRWLockExclusive(rwlock); return 0;
}

int rwlock_rdunlock(RWLock *rwlock)
{
   ReleaseSRWLockShared(rwlock); return 0;
}

int rwlock_wrunlock(RWLock *rwlock)
{
   ReleaseSRWLockExclusive(rwlock); return 0;
}

int rwlock_free(RWLock *rwlock)
{  /* A SRWLock does not need to be explicitly destroyed, so this
    * function simply tries to acquire an exclusive lock on *rwlock
    * and, either immediately releases the lock, or returns the
    * EBUSY code, per pthread_rwlock_destory() functionality. */
   if (TryAcquireSRWLockExclusive(rwlock)) {
      ReleaseSRWLockExclusive(rwlock);
      return 0;
   } else return EBUSY;
}

#endif  /* end Windows */

/* Wait for list of threads, tidlist[count], to complete. (BLOCKING)
 * Returns 0 on success, else the first error code from thread_wait(). */
int thread_multiwait(int count, ThreadId tidlist[])
{
   int i, temp, ecode;

   for(ecode = temp = i = 0; i < count; i++) {
      temp = thread_wait(tidlist[i]);
      if(temp && !ecode) ecode = temp;
   }

   return ecode;
}


#endif  /* end _EXTENDED_THREAD_C_ */
