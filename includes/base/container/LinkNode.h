#ifndef LINK_NODE_H
#define LINK_NODE_H

#include <stdlib.h>

template <class T>
class LinkNode
{
public:
   
   LinkNode(size_t key, T value);
   
   void           next(LinkNode<T>* pNext);
   LinkNode<T>*   next();
   
   void           prev(LinkNode<T>* pPrev);
   LinkNode<T>*   prev();
   
   T     value();
   
   size_t   key();
   
private:
   
   LinkNode<T>* m_pNext;
   LinkNode<T>* m_pPrev;
   
   T m_Value;
   
   size_t   m_nKey;
};

//------------------------------------------------------------------------------
template <class T>
LinkNode<T>::LinkNode(size_t key, T value)
{
   m_nKey   = key;
   m_pNext  = NULL;
   m_pPrev  = NULL;
   m_Value  = value;
}

//------------------------------------------------------------------------------
template <class T>
void LinkNode<T>::next(LinkNode<T>* pNext)
{
   m_pNext = pNext;
}

//------------------------------------------------------------------------------
template <class T>
LinkNode<T>* LinkNode<T>::next()
{
   return m_pNext;
}

//------------------------------------------------------------------------------
template <class T>
void LinkNode<T>::prev(LinkNode<T>* pPrev)
{
   m_pPrev = pPrev;
}

//------------------------------------------------------------------------------
template <class T>
LinkNode<T>* LinkNode<T>::prev()
{
   return m_pPrev;
}

//------------------------------------------------------------------------------
template <class T>
T LinkNode<T>::value()
{
   return m_Value;
}

//------------------------------------------------------------------------------
template <class T>
size_t LinkNode<T>::key()
{
   return m_nKey;
}

#endif // SINGLE_LINK_NODE_H