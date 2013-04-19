#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <queue>
#include <stdio.h>
#include "CountingSem.h"
#include "Mutex.h"

template <class T>
class Queue
{
public:
   
   static const unsigned int InfiniteQueue = 0;
   
   Queue();
   
   ~Queue();
   
   bool  initialize(unsigned int nMaxElements = InfiniteQueue);
   
   bool  push(const T &item, int nTimeoutMs = Sem::SemWaitForever);
   
   bool  pop(T &item, int nTimeoutMs = Sem::SemWaitForever);
   
   bool  isEmpty();
   
   unsigned int size();
   
private:
   
   Queue(const Queue& src);
   
   Queue & operator=(const Queue &rhs);
   
private:
   
   unsigned int   m_nMaxSize;
   
   std::queue<T>  m_queue;
   
   CountingSem* m_pPushSem;
   
   CountingSem* m_pPopSem;
};


//------------------------------------------------------------------------------
template <class T>
Queue<T>::Queue()
{
   m_nMaxSize = 0;
   
   m_pPushSem = NULL;
   m_pPopSem = NULL;
}

//------------------------------------------------------------------------------
template <class T>
Queue<T>::~Queue()
{   
   if (m_pPushSem)
   {
      delete m_pPushSem;
      m_pPushSem = NULL;
   }
   
   if (m_pPopSem)
   {
      delete m_pPopSem;
      m_pPopSem = NULL;
   }
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::initialize(unsigned int nMaxElements)
{
   m_nMaxSize = nMaxElements;
   
   m_pPushSem = new CountingSem(0);
   if (m_pPushSem == NULL)
   {
      return false;
   }
   
   m_pPopSem = new CountingSem(m_nMaxSize);
   if (m_pPopSem == NULL)
   {
      delete m_pPushSem;
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::push(const T &item, int nTimeoutMs)
{
   bool  l_bSuccess = false;
      
   // The producer that acquires the push lock now needs to wait for room.
   if (m_pPopSem->take(nTimeoutMs) == Sem::SemAcquired)
   {
      l_bSuccess = true;
      
      // Otherwise, the item can pushed onto the end of the queue.
      m_queue.push(item);
      
      // Post the push semaphore
      m_pPushSem->give();      
   }
      
   return l_bSuccess;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::pop(T &item, int nTimeoutMs)
{
   bool  l_bSuccess = false;
      
   if (m_pPushSem->take(nTimeoutMs) == Sem::SemAcquired)
   {
      item = m_queue.front();
      m_queue.pop();
      
      m_pPopSem->give();
            
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::isEmpty()
{
   return m_queue.empty();
}

//------------------------------------------------------------------------------
template <class T>
unsigned int Queue<T>::size()
{
   return (unsigned int)m_queue.size();
}

#endif // QUEUE_H
