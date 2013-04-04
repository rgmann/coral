#ifndef QUEUE_H
#define QUEUE_H

#include <string>
#include <queue>
#include "BinarySem.h"

template <class T>
class Queue
{
public:
   
   static const unsigned int InfiniteQueue = 0;
   
   Queue(unsigned int nMaxElements = InfiniteQueue);
   
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
   
   BinarySem m_pushSem;
   
   BinarySem m_popSem;
};


//------------------------------------------------------------------------------
template <class T>
Queue<T>::Queue(unsigned int nMaxElements)
{
   m_nMaxSize = nMaxElements;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::push(const T &item, int nTimeoutMs)
{
   bool l_bWaitForSpace =  (m_nMaxSize != InfiniteQueue) &&
                           (m_queue.size() >= m_nMaxSize);
   
   // If we need to wait for space (not an infinite queue), and we fail to
   // acquire the semaphore, then we return with adding the item.
   if (l_bWaitForSpace &&
       (m_popSem.take(nTimeoutMs) != Sem::SemAcquired))
   {
      return false;
   }
   
   // Otherwise, the item can pushed onto the end of the queue.
   m_queue.push(item);
   
   // Post the push semaphore
   m_pushSem.give();
   
   return true;
}

//------------------------------------------------------------------------------
template <class T>
bool Queue<T>::pop(T &item, int nTimeoutMs)
{
   if ((m_queue.size() == 0) &&
       (m_pushSem.take(nTimeoutMs) != Sem::SemAcquired))
   {
      return false;
   }
   
   if (isEmpty()) return false;
   
   item = m_queue.front();
   m_queue.pop();
   
   m_popSem.give();
   
   return true;
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