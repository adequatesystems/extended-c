
#include "_assert.h"
#include "extthrd.h"
#include "exttime.h"

#define NUMTHREADS 123
#define ITERATIONS 123

Condition si_cond = CONDITION_INITIALIZER;
RWLock si_rwlock = RWLOCK_INITIALIZER;
Mutex si_mutex = MUTEX_INITIALIZER;
ThreadId si_id;

ThreadProc check_id(void *ignore)
{
   ThreadId id = thread_selfid();

   (void)ignore;

   ASSERT_NE(id, si_id);

   Unthread;
}

ThreadProc condition_counter(void *args)
{
   int *counter = (int *) args;

   mutex_lock(&si_mutex);
   condition_wait(&si_cond, &si_mutex);
   (*counter)++;
   mutex_unlock(&si_mutex);

   Unthread;
}

ThreadProc exclusive_rwlock_counter(void *args)
{
   int *counter = (int *) args;
   int i;

   for (i = 0; i < ITERATIONS; i++) {
      rwlock_wrlock(&si_rwlock);
      (*counter)++;
      rwlock_wrunlock(&si_rwlock);
   }

   Unthread;
}

ThreadProc shared_rwlock_counter(void *args)
{
   int *counter = (int *) args;
   int i;

   for (i = 0; i < ITERATIONS; i++) {
      rwlock_rdlock(&si_rwlock);
      (*counter)++;
      rwlock_rdunlock(&si_rwlock);
   }

   Unthread;
}

ThreadProc rwlock_tryrdcounter(void *args)
{
   int *counter = (int *) args;

   if (rwlock_tryrdlock(&si_rwlock) == 0) {
      (*counter)++;
      rwlock_rdunlock(&si_rwlock);
   }

   Unthread;
}

ThreadProc rwlock_trywrcounter(void *args)
{
   int *counter = (int *) args;

   if (rwlock_trywrlock(&si_rwlock) == 0) {
      (*counter)++;
      rwlock_wrunlock(&si_rwlock);
   }

   Unthread;
}

ThreadProc mutex_counter(void *args)
{
   int *counter = (int *) args;
   int i;

   for (i = 0; i < ITERATIONS; i++) {
      mutex_lock(&si_mutex);
      (*counter)++;
      mutex_unlock(&si_mutex);
   }

   Unthread;
}

ThreadProc mutex_trycounter(void *args)
{
   int *counter = (int *) args;

   if (mutex_trylock(&si_mutex) == 0) {
      (*counter)++;
      mutex_unlock(&si_mutex);
   }

   Unthread;
}

/* this function serves ONLY to test the ThreadRoutine datatype */
int startThread(Thread *thrdp, ThreadRoutine func, void *args)
{  return thread_create(thrdp, func, args);  }

int main()
{
   Thread thrd[NUMTHREADS];
   Condition cond;
   RWLock rwlock;
   Mutex mutex;
   int counter;
   int i, old;

   thread_setname(thread_self(), "main_thread");

   /* check local init and destroy */
   ASSERT_EQ(condition_init(&cond), 0);
   ASSERT_EQ(rwlock_init(&rwlock), 0);
   ASSERT_EQ(mutex_init(&mutex), 0);
   ASSERT_EQ(mutex_destroy(&mutex), 0);
   ASSERT_EQ(rwlock_destroy(&rwlock), 0);
   ASSERT_EQ(condition_destroy(&cond), 0);

   /* spawn NUMTHREADS threads to add ITERATIONS to counter -- mutex */
   counter = 0;
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(startThread(&thrd[i], mutex_counter, &counter), 0);
   }  /* join threads */
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(thread_join(thrd[i]), 0);
   }  /* check counter -- (NUMTHREADS * ITERATIONS) */
   ASSERT_EQ(counter, (NUMTHREADS * ITERATIONS));

   /* spawn NUMTHREADS threads to add ITERATIONS to counter -- rwlock */
   counter = 0;
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(startThread(&thrd[i], exclusive_rwlock_counter, &counter), 0);
   }  /* join threads */
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(thread_join(thrd[i]), 0);
   }  /* check counter -- (NUMTHREADS * ITERATIONS) */
   ASSERT_EQ(counter, (NUMTHREADS * ITERATIONS));

   /* spawn NUMTHREADS threads to add ITERATIONS to counter -- rwlock */
   counter = 0;
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(startThread(&thrd[i], shared_rwlock_counter, &counter), 0);
   }  /* join threads */
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(thread_join(thrd[i]), 0);
   }  /* check counter -- (NUMTHREADS * ITERATIONS) */
   /* THIS ASSUMPTION CANNOT BE MADE FOR CERTAIN...
   ASSERT_EQ(counter, (NUMTHREADS * ITERATIONS)); */

   /* spawn NUMTHREADS threads to wait on condition count */
   counter = 0;
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(startThread(&thrd[i], condition_counter, &counter), 0);
   }
   /* wait for threads to start */
   millisleep(1000);
   /* signal condition and check */
   mutex_lock(&si_mutex);
   condition_signal(&si_cond);
   ASSERT_EQ(counter, 0);
   mutex_unlock(&si_mutex);
   condition_signal(&si_cond);
   /* wait for condition signal effect */
   millisleep(1000);
   mutex_lock(&si_mutex);
   ASSERT_GT(counter, 0);
   old = counter;
   mutex_unlock(&si_mutex);
   /* broadcast condition and check */
   condition_broadcast(&si_cond);
   /* wait for threads */
   for(i = 0; i < NUMTHREADS; i++) {
      ASSERT_EQ(thread_join(thrd[i]), 0);
   }
   mutex_lock(&si_mutex);
   ASSERT_GT(counter, old);
   /* check condition timed wait timeout while we're here... */
   ASSERT_NE(condition_timedwait(&si_cond, &si_mutex, 100), 0);

   /* do mutex trylock in another thread */
   counter = 0;
   ASSERT_EQ(startThread(thrd, mutex_trycounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 0);
   /* unlock and try again */
   ASSERT_EQ(mutex_unlock(&si_mutex), 0);
   ASSERT_EQ(startThread(thrd, mutex_trycounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_GT(counter, 0);

   /* do rwlock trylocks in another thread */
   ASSERT_EQ(rwlock_wrlock(&si_rwlock), 0);
   counter = 0;
   ASSERT_EQ(startThread(thrd, rwlock_trywrcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 0);
   ASSERT_EQ(startThread(thrd, rwlock_tryrdcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 0);
   /* unlock and try again */
   ASSERT_EQ(rwlock_wrunlock(&si_rwlock), 0);
   ASSERT_EQ(startThread(thrd, rwlock_trywrcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 1);
   /* ... again with rdlock */
   ASSERT_EQ(rwlock_rdlock(&si_rwlock), 0);
   counter = 0;
   ASSERT_EQ(startThread(thrd, rwlock_trywrcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 0);
   ASSERT_EQ(startThread(thrd, rwlock_tryrdcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 1);
   /* unlock and try again */
   ASSERT_EQ(rwlock_rdunlock(&si_rwlock), 0);
   ASSERT_EQ(startThread(thrd, rwlock_tryrdcounter, &counter), 0);
   ASSERT_EQ(thread_join(thrd[0]), 0);
   ASSERT_EQ(counter, 2);

   si_id = thread_selfid();
   Thread self = thread_self();
   Thread other;
   ASSERT_EQ(thread_create(&other, check_id, NULL), 0);
   ASSERT_EQ(thread_equal(self, other), 0);
   ASSERT_EQ(thread_equal(self, self), 1);
   ASSERT_EQ(thread_cancel(other), 0);
   /* unsafe to proceed after thread_cancel() */
   return 0;
}
