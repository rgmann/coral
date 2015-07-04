#include <pthread.h>
#include <errno.h>
#include <string.h>
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
   m_pSem = NULL;
   
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

//         if (l_nStatus == 0 || l_nStatus == ETIMEDOUT)
//            break;
      }
   }

   // Decrement the waiters count.
   m_nConsumerCount--;
   
   if (l_nStatus == 0)
   {
      // <s->lock_> is now held.

      // Decrement the semaphore's count.
      m_nCount--;
   }
   
   // Release mutex to leave critical section.
   pthread_mutex_unlock(&m_pSem->lock);
   
   return (l_nStatus == 0) ? SemAcquired : SemTimeout;
}

//------------------------------------------------------------------------------
Sem::SemStatus CountingSem::give()
{
   int l_nStatus = 0;
   
   pthread_mutex_lock(&m_pSem->lock);
      
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
