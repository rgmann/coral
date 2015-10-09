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



#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "Thread.h"

Mutex Thread::ourIdMutex;
ui32  Thread::ournCurrentTid = 0;

struct ThreadPriv
{
   pthread_t      thread;
};

//------------------------------------------------------------------------------
Thread::Thread()
{
   m_bStarted = false;
   m_bDone = false;
   
   m_pThread = NULL;
   m_pThread = new ThreadPriv();
   if (m_pThread)
   {
      memset(&m_pThread->thread, 0, sizeof(m_pThread->thread));
   }
   
   m_pThreadArg = NULL;
   m_pUserFunc = NULL;
}

//------------------------------------------------------------------------------
Thread::~Thread()
{
   if (m_pThread)
   {
      delete m_pThread;
      m_pThread = NULL;
   }
}

//------------------------------------------------------------------------------
Thread* Thread::Create(UserThreadFunc func, void* pArg)
{
   int l_nRetVal = 0;
   
   Thread* l_pThread = NULL;
   
   l_pThread = new Thread();
   if (l_pThread == NULL)
   {
      printf("Thread::Create: Failed to allocate thread.\n");
      return NULL;
   }
   
   // Assign a thread ID.
   ourIdMutex.lock();
   l_pThread->m_nId = ++ournCurrentTid;
   ourIdMutex.unlock();
   
   l_pThread->m_pUserFunc = func;
   l_pThread->m_pThreadArg = new ThreadArg();
   if (l_pThread->m_pThreadArg == NULL)
   {
      printf("Thread::Create: Failed to allocate ThreadArg.\n");
      return NULL;
   }
   l_pThread->m_pThreadArg->pUserData = pArg;
   
   l_nRetVal = pthread_create(&l_pThread->m_pThread->thread,
                              NULL,                      // No attributes
                              ThreadFunc,                // Thread function
                              (void*)l_pThread);        // Thread args
   if (l_nRetVal != 0)
   {
      perror("PThread::Create: Error creating POSIX thread - ");
      
      delete l_pThread;
      l_pThread = NULL;
   }

   return l_pThread;
}

//------------------------------------------------------------------------------
bool Thread::join(ThreadRet* pRet)
{
   int l_nRetVal = 0;
   
   // Signal that the thread should stop
   if (!stop())
   {
      return true;
   }
   
   if (pRet)
   {
      l_nRetVal = pthread_join(m_pThread->thread, (void**)&pRet);
   }
   else
   {
      l_nRetVal = pthread_join(m_pThread->thread, NULL);
   }

   return (l_nRetVal == 0);
}

//------------------------------------------------------------------------------
bool Thread::stop()
{
   bool  l_nRetVal = false;
   
   if (isActive())
   {
      m_pThreadArg->m_bStopSignal = true;
      l_nRetVal = true;;
   }
   
   return l_nRetVal;
}

//------------------------------------------------------------------------------
ui32 Thread::getId() const
{
   return m_nId;
}

//------------------------------------------------------------------------------
void* Thread::ThreadFunc(void* pArg)
{
   Thread* l_pThread = NULL;
   
   if (pArg == NULL)
   {
      printf("Thread error: pArg == NULL\r\n");
      return NULL;
   }
   
   l_pThread = (Thread*)pArg;
   
   // Push the cleanup handler.
   pthread_cleanup_push(CleanupHandler, l_pThread);
   
   // Signal that the thread has started.
   l_pThread->m_bStarted = true;
   
   // Invoke the user specified thread function.
   l_pThread->m_pUserFunc(l_pThread->m_pThreadArg);
   
   // At this point the user thread has finished.
   // Invoke cleanup handler.
   pthread_cleanup_pop(1);
   return NULL;
}

//------------------------------------------------------------------------------
void Thread::CleanupHandler(void* pArg)
{
   Thread* l_pThread = NULL;
   
   if (pArg == NULL)
   {
      return;
   }
   
   l_pThread = (Thread*)pArg;
   
   // Signal that the thread is done.
   l_pThread->m_bDone = true;
}

//------------------------------------------------------------------------------
bool Thread::isActive() const
{
   return (!m_bDone && m_bStarted);
}
