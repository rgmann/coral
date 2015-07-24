#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <queue>
#include <stdio.h>
#include "BaseTypes.h"
#include "CountingSem.h"
#include "Mutex.h"

template <class T>
class Queue {
public:
   
   static const ui32 InfiniteQueue = 0;
   
   Queue();
   
   ~Queue();
   
   bool  initialize(ui32 nMaxElements = InfiniteQueue);
   
   bool  push(const T &item, int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);
   
   bool  pop(T &item, int nTimeoutMs = liber::thread::Semaphore::SemWaitForever);
   
   bool  peek(T &item);
   
   bool  isEmpty();
   
   ui32 size();
   
private:
   
   Queue(const Queue& src);
   
   Queue & operator=(const Queue &rhs);
   
private:
   
   ui32  m_nMaxSize;
   bool  m_bIsInfinite;
   
   std::queue<T>  m_queue;
   
   CountingSem* m_pPushSem;
   
   CountingSem* m_pPopSem;

   Mutex m_queueLock;
};


//------------------------------------------------------------------------------
template <class T>
Queue<T>::Queue()
{
   m_nMaxSize = 0;
   m_bIsInfinite = false;
   
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
bool Queue<T>::initialize(ui32 nMaxElements)
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
   
   // If this is an "infinite" queue, we simply don't use the pop semaphore.
   m_bIsInfinite = (nMaxElements == InfiniteQueue);
   
   return true;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::push(const T &item, int nTimeoutMs)
{
   bool  l_bSuccess = false;
      
   // The producer that acquires the push lock now needs to wait for room.
   if (!m_bIsInfinite && (m_pPopSem->take(nTimeoutMs) != liber::thread::Semaphore::SemAcquired))
   {
      return false;
   }
//   if (m_pPopSem->take(nTimeoutMs) == Sem::SemAcquired)
//   {
      l_bSuccess = true;
      
      // Otherwise, the item can pushed onto the end of the queue.
      m_queueLock.lock();
      m_queue.push(item);
      m_queueLock.unlock();
      
      // Post the push semaphore
      m_pPushSem->give();      
//   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::pop(T &item, int nTimeoutMs)
{
   bool  l_bSuccess = false;
      
   if (m_pPushSem->take(nTimeoutMs) == liber::thread::Semaphore::SemAcquired)
   {
      m_queueLock.lock();
      item = m_queue.front();
      m_queue.pop();
      m_queueLock.unlock();
      
//      m_pPopSem->give();
      if (!m_bIsInfinite) m_pPopSem->give();
      
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::peek(T &item)
{
   bool l_bSuccess = false;
   
   if (!isEmpty())
   {
      m_queueLock.lock();
      item = m_queue.front();
      m_queueLock.unlock();
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::isEmpty()
{
   m_queueLock.lock();
   bool l_bEmpty = m_queue.empty();
   m_queueLock.unlock();
   return l_bEmpty;
}

//------------------------------------------------------------------------------
template <class T>
ui32 Queue<T>::size()
{
   m_queueLock.lock();
   ui32 l_nSize = m_queue.size();
   m_queueLock.unlock();
   return l_nSize;
}

#endif // QUEUE_H
