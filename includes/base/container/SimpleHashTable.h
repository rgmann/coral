#ifndef SIMPLE_HASH_TABLE_H
#define SIMPLE_HASH_TABLE_H

#include "LinkNode.h"
#include <stdio.h>
#include <vector>

template <size_t N, class T>
class SimpleHashTable {
public:
   
  SimpleHashTable();
   
  ~SimpleHashTable();
   
  bool add(size_t key, T val, bool bAllowDups = false);

  bool get(size_t key, std::vector<T>& matches);

  void remove(size_t key);
   
  ui32 count() const;

private:
   
   size_t hash(size_t key);
   
   void printChain(LinkNode<T>* pHead);
   
private:
   
   static const size_t Modulus = (1 << N);
   
   LinkNode<T>* m_HashArray[Modulus];
   
   unsigned int m_nElementCount;
};

// NOTE: hashes the clients blocks.  Server looks up rolling checksum in hash
// as the first level search.  If the there is no entry in the hash, then there
// is no match.  If there is a match, then you perform second level search.

//------------------------------------------------------------------------------
template <size_t N, class T>
SimpleHashTable<N,T>::SimpleHashTable()
{
  m_nElementCount = 0;
   
  for (size_t l_nInd = 0; l_nInd < Modulus; l_nInd++)
  {
    m_HashArray[l_nInd] = NULL;
  }
}

//------------------------------------------------------------------------------
template <size_t N, class T>
SimpleHashTable<N,T>::~SimpleHashTable()
{
  // TODO: Delete hash table chains.
  LinkNode<T>* pHead = NULL;
   
  for (size_t l_nInd = 0; l_nInd < Modulus; ++l_nInd)
  {
    // Reel in each chain
#ifdef DEBUG_CHAIN
    printf("{D:h=%lu}",l_nInd);printChain(m_HashArray[l_nInd]);
#endif
    while (m_HashArray[l_nInd] != NULL)
    {
      pHead = m_HashArray[l_nInd];

      if (pHead)
      {
        m_HashArray[l_nInd] = pHead->next();

        delete pHead;
        pHead = NULL;
      }
#ifdef DEBUG_CHAIN
      printf("{D:h=%lu}",l_nInd);printChain(m_HashArray[l_nInd]);
#endif
    }
  }
}

//------------------------------------------------------------------------------
template <size_t N, class T>
bool SimpleHashTable<N,T>::add(size_t key, T val, bool bAllowDups)
{
  size_t l_nHash = 0;
  bool l_bNodeAdded = false;

  LinkNode<T>**  pHead = NULL;
  LinkNode<T>*   pTemp = NULL;

  l_nHash = hash(key);
  pHead = &m_HashArray[l_nHash];

  do {
    if (*pHead == NULL)
    {
      (*pHead) = new LinkNode<T>(key, val);
      l_bNodeAdded = true;
      break;
    }
      
    // If duplicate keys are forbidden and we've found a duplicate key,
    // we're done.
    if (!bAllowDups && (*pHead)->key() == key)
    {
      break;
    }

    if ((*pHead)->next() == NULL)
    {
      (*pHead)->next(new LinkNode<T>(key, val));
      l_bNodeAdded = true;
      break;
    }

    pTemp = (*pHead)->next();
    pHead = &pTemp;
  } while (1);
   
  if (l_bNodeAdded)
  {
    m_nElementCount++;
  }

#ifdef DEBUG_CHAIN
  printf("{A:h=%lu,k=%lu}",l_nHash,key);printChain(m_HashArray[l_nHash]);
#endif
   
  return l_bNodeAdded;
}

//------------------------------------------------------------------------------
template <size_t N, class T>
bool SimpleHashTable<N,T>::get(size_t key, std::vector<T> &matches)
{
  bool  l_bFound = false;
  size_t l_nHash = 0;
  LinkNode<T>*  pHead = NULL;

  l_nHash = hash(key);
  pHead = m_HashArray[l_nHash];
   
  // We have not yet found the key.
  while (1)
  {
    if (pHead == NULL)
    {
      break;
    }
      
    if (pHead->key() == key)
    {
      l_bFound = true;
      matches.push_back(pHead->value());
    }
      
    pHead = pHead->next();
  }
   
#ifdef DEBUG_CHAIN
   printf("{G:h=%lu,k=%lu}",l_nHash,key);printChain(m_HashArray[l_nHash]);
#endif
   
  return (matches.empty() == false);
}

//------------------------------------------------------------------------------
template <size_t N, class T>
unsigned int SimpleHashTable<N,T>::count() const
{
   return m_nElementCount;
}

//------------------------------------------------------------------------------
template <size_t N, class T>
size_t SimpleHashTable<N,T>::hash(size_t key)
{
   return (key % Modulus);
}

//------------------------------------------------------------------------------
template <size_t N, class T>
void SimpleHashTable<N,T>::printChain(LinkNode<T>* pHead)
{
   printf("(*)");
   
   while (1)
   {
      if (pHead == NULL)
      {
         printf("->NULL");
         break;
      }
      
      printf("->[k:%lu]", pHead->key());
      pHead = pHead->next();
   }
   printf("\n");
}

#endif // SIMPLE_HASH_H
