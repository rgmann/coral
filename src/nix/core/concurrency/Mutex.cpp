// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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
