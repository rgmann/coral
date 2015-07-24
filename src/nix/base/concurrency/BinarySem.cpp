#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <boost/thread/thread_time.hpp>
#include <boost/thread/locks.hpp> 
#include "BinarySem.h"

// Based on:
//http://www.mathcs.emory.edu/~cheung/Courses/455/Syllabus/5c-pthreads/sync.html

// struct SemPriv
// {
//    pthread_cond_t    cond;
//    pthread_mutex_t   lock;
// };

//------------------------------------------------------------------------------
BinarySem::BinarySem(BinarySemState start)
: count_( false )
{
   // m_pSem = NULL;
   // m_pSem = new SemPriv();
   
   // if (m_pSem)
   // {
   //    pthread_mutex_init(&m_pSem->lock, NULL);
   //    pthread_cond_init(&m_pSem->cond, NULL);
   // }
   
   // m_bIsTaken = (start == BinarySemDown) ? false : true;
}

//------------------------------------------------------------------------------
BinarySem::~BinarySem()
{
   // Copy pointer and set m_pMutex to NULL so it can't be locked again.
   // SemPriv* l_pSem = m_pSem;
   // l_pSem = NULL;
   
   // Delete the mutex by way of the copied pointer.
   // if (l_pSem)
   // {
   //    pthread_mutex_destroy(&l_pSem->lock);
   //    pthread_cond_destroy(&l_pSem->cond);
   //    delete l_pSem;
   //    l_pSem = NULL;
   // }
}

//------------------------------------------------------------------------------
// Sem::SemStatus BinarySem::take(int nTimeoutMs)
// {
//    int l_nStatus = 0;
//    struct timeval    tv;
//    struct timespec   ts;
   
//    gettimeofday(&tv, NULL);
//    ts.tv_sec = tv.tv_sec + (nTimeoutMs / 1000);
//    ts.tv_nsec = tv.tv_usec * 1000 + 1000000 * (nTimeoutMs % 1000);
   
//    /* Try to get exclusive access to s->flag */
//    pthread_mutex_lock(&(m_pSem->lock));
   
//    /* Success - no other thread can get here unless
//     the current thread unlock "mutex"              */
   
//    /* Examine the flag and wait until flag == 1 */
//    while (!m_bIsTaken)
//    {
//       if (nTimeoutMs == Sem::SemWaitForever)
//       {
//          l_nStatus = pthread_cond_wait(&m_pSem->cond, &m_pSem->lock);
//       }
//       else
//       {
//          l_nStatus = pthread_cond_timedwait(&m_pSem->cond, &m_pSem->lock, &ts);
         
//          if (l_nStatus == ETIMEDOUT)
//          {
//             break;
//          }
//       }
//       /* When the current thread execute this
//        pthread_cond_wait() statement, the
//        current thread will be block on s->cv
//        and (atomically) unlocks s->mutex !!!
//        Unlocking s->mutex will let other thread
//        in to test s->flag.... */
//    }
   
//    if (l_nStatus == 0)
//    {
//       /* -----------------------------------------
//        If the program arrive here, we know that
//        s->flag == 1 and this thread has now
//        successfully pass the semaphore !!!
//        ------------------------------------------- */
//       m_bIsTaken = false;  /* This will cause all other threads
//                             that executes a P() call to wait
//                             in the (above) while-loop !!! */
//    }
   
//    /* Release exclusive access to s->flag */
//    pthread_mutex_unlock(&(m_pSem->lock));
   
//    return (l_nStatus == 0) ? SemAcquired : SemTimeout;
// }
Sem::SemStatus BinarySem::take( i32 nTimeoutMs )
{
   bool got_sem = true;

   boost::mutex::scoped_lock guard( lock_ );
   
   while ( count_ == 0 )
   {
      if ( nTimeoutMs == Sem::SemWaitForever )
      {
         condition_.wait( guard );
      }
      else
      {
         boost::system_time const timeout =
            boost::get_system_time() +
            boost::posix_time::milliseconds( nTimeoutMs );

         if ( ( got_sem = condition_.timed_wait( guard, timeout ) ) )
         {
            break;
         }
      }
   }

   // Decrement the waiters count.
   if ( got_sem ) count_ = 0;
   
   return got_sem ? SemAcquired : SemTimeout;
}

//------------------------------------------------------------------------------
Sem::SemStatus BinarySem::give()
{
  boost::mutex::scoped_lock guard( lock_ );

  count_ = 1;
  condition_.notify_one();
   
  return SemOk;
}
