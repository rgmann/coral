#include <pthread.h>
#include <errno.h>
//#include <time.h>
//#include <sys/time.h>
#include <stdio.h>
#include "CountingSem.h"

// Based on: http://www.cse.wustl.edu/~schmidt/win32-cv-1.html

struct SemPriv
{
   pthread_cond_t    cond;
   pthread_mutex_t   lock;
};

//------------------------------------------------------------------------------
CountingSem::CountingSem(int initial)
{
   m_pSem = new SemPriv();
   
   if (m_pSem)
   {
      pthread_mutex_init(&m_pSem->lock, NULL);
      pthread_cond_init(&m_pSem->cond, NULL);
   }
   
   m_nCount = initial;
   m_nConsumerCount = 0;
}

//------------------------------------------------------------------------------
CountingSem::~CountingSem()
{
   // Copy pointer and set m_pMutex to NULL so it can't be locked again.
   SemPriv* l_pSem = m_pSem;
   l_pSem = NULL;
   
   // Delete the mutex by way of the copied pointer.
   if (l_pSem)
   {
      pthread_mutex_destroy(&l_pSem->lock);
      pthread_cond_destroy(&l_pSem->cond);
      
      delete l_pSem;
      l_pSem = NULL;
   }
}

//------------------------------------------------------------------------------
Sem::SemStatus CountingSem::take(int nTimeoutMs)
{
   int l_nStatus = 0;
   struct timespec   ts;
   
   SetTimeoutTime(&ts, nTimeoutMs);
   
   // Acquire mutex to enter critical section.
   pthread_mutex_lock(&m_pSem->lock);

   // Keep track of the number of waiters so that <sema_post> works correctly.
   m_nConsumerCount++;
   
   printf("CountingSem::take: consumers = %d, count = %d\n",
          m_nConsumerCount, m_nCount);

   // Wait until the semaphore count is > 0, then atomically release
   // <lock_> and wait for <count_nonzero_> to be signaled.
   while (m_nCount == 0)
   {
      if (nTimeoutMs == Sem::SemWaitForever)
      {
         l_nStatus = pthread_cond_wait(&m_pSem->cond, &m_pSem->lock);
      }
      else
      {
         l_nStatus = pthread_cond_timedwait(&m_pSem->cond,
                                            &m_pSem->lock, &ts);
         
         if ((l_nStatus == 0 && errno != EINTR) ||
             l_nStatus == ETIMEDOUT)
         {
            break;
         }
      }
   }

   // Decrement the waiters count.
   m_nConsumerCount--;
   
   printf("CountingSem::take: consumers = %d, status = %d\n",
          m_nConsumerCount, l_nStatus);
   
   if (l_nStatus == 0)
   {
      // <s->lock_> is now held.

      // Decrement the semaphore's count.
      m_nCount--;
      
      // Release mutex to leave critical section.
      pthread_mutex_unlock(&m_pSem->lock);
   }
   
   printf("CountingSem::take: TOOK consumers = %d, count = %d\n",
          m_nConsumerCount, m_nCount);
   
   return (l_nStatus == 0) ? SemAcquired : SemTimeout;
}

//------------------------------------------------------------------------------
Sem::SemStatus CountingSem::give()
{
   int l_nStatus = 0;
   
   printf("CountingSem::give: entry\n");
   pthread_mutex_lock(&m_pSem->lock);
   
   printf("CountingSem::give: got lock - consumers = %d\n", m_nConsumerCount);
   
   // Always allow one thread to continue if it is waiting.
   if (m_nConsumerCount > 0)
   {
      l_nStatus = pthread_cond_signal(&m_pSem->cond);
   }
   
   // Increment the semaphore's count.
   m_nCount++;
   
   pthread_mutex_unlock(&m_pSem->lock);
   
   return (l_nStatus == 0) ? SemOk : SemError;
}
