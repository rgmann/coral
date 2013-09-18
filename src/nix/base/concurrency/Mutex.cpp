#include "Mutex.h"
#include "Timestamp.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

struct PlatformMutex
{
   pthread_mutexattr_t mAttr;
   pthread_mutex_t     m;
};

//------------------------------------------------------------------------------
Mutex::Mutex()
{
   m_pMutex = NULL;
   m_pMutex = new PlatformMutex();
   
   if (m_pMutex)
   {
      pthread_mutexattr_init(&m_pMutex->mAttr);
      pthread_mutexattr_settype(&m_pMutex->mAttr, PTHREAD_MUTEX_ERRORCHECK);
      pthread_mutex_init(&m_pMutex->m, &m_pMutex->mAttr);
   }
}

//------------------------------------------------------------------------------
Mutex::~Mutex()
{
   // Copy pointer and set m_pMutex to NULL so it can't be locked again.
   PlatformMutex* l_pMutex = m_pMutex;
   m_pMutex = NULL;
   
   // Delete the mutex by way of the copied pointer.
   if (l_pMutex)
   {
      pthread_mutex_destroy(&l_pMutex->m);
      delete l_pMutex;
      l_pMutex = NULL;
   }
}

//------------------------------------------------------------------------------
bool  Mutex::lock(i32 nTimeoutMs)
{
   int locRetVal = 0;
   
   if (m_pMutex == NULL)
   {
      return false;
   }
   
   if (nTimeoutMs == WaitForever)
   {
      locRetVal = pthread_mutex_lock(&m_pMutex->m);
   }
   else
   {
      Timestamp start, elapsed;
      
      start.sample();
      elapsed.sample();
      
      while (elapsed.diffInMs(start) < nTimeoutMs)
      {
         locRetVal = pthread_mutex_trylock(&m_pMutex->m);
         
         if (locRetVal == 0)
         {
            break;
         }
         
         sleep(5);
         
         elapsed.sample();
      }
   }
   
   return (locRetVal == 0);
}

//------------------------------------------------------------------------------
bool  Mutex::unlock()
{
   int locRetVal = 0;
   
   if (m_pMutex == NULL)
   {
      return false;
   }
   
   locRetVal = pthread_mutex_unlock(&m_pMutex->m);
   
   return (locRetVal == 0);
}
