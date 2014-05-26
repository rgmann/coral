#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <cmath>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include "Timestamp.h"

#define DISABLED  0
#define ENABLED   1
#define PROFILE DISABLED

namespace liber {

template<int KeySize, typename Type>
class HashTable {
public:

  HashTable();
  ~HashTable();

  void insert(int key, Type value);

  template<typename Comparator>
  bool remove(int key, Type& value, Comparator& comp);

  template<typename Comparator>
  bool find(int key, Type& item, Comparator& comp);


  void clear();

  template<typename Destroyer>
  void clear(Destroyer& destroy);

  /**
   * Get the number of items stored in the HashTable.
   */
  inline size_t size() const { return mnItemCount; };

  inline size_t buckets() const { return mnBucketCount; };

private:

  typedef std::pair<int, Type> Item;
  typedef std::vector<Item> Bucket;

  /**
   * Extremely simple hash function.
   */
  size_t hash(int key) { return (size_t)(key % mnBucketCount); };

  /**
   * Access an item by item by key and comparison functor.
   *
   * @param key[int]    - Hash table key
   * @param value[Type] - If an item is found, it returned in value. The value
   *                      reference is not modified unless an item is found.
   * @param compare[Comparator] - Item comparison functor
   *
   * @return pair<size_t, bucket> - Returns a pair where "first" is the bucket
   *                      index (i.e. hash key) and "second" is an iterator
   *                      pointing to the items position in the bucket indexed
   *                      by "first".
   */
  template<typename Comparator>
  std::pair<size_t, typename Bucket::iterator> get(int, Type& value, Comparator&);

  std::ofstream mLog;

private:

  // Hash table bucket count.
  size_t mnBucketCount;

  // The hash table is a vector of vectors.
  std::vector<Bucket> mBuckets;

  // Number of items currently stored in the hash table.
  size_t mnItemCount;
};

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
HashTable<KeySize, Type>::HashTable()
: mnItemCount(0)
{
  mnBucketCount = (int)std::pow(2, KeySize);

  for (int nBucket = 0; nBucket < mnBucketCount; nBucket++)
  {
    mBuckets.push_back(Bucket());
  }

#if PROFILE == ENABLED
  mLog.open("/Users/vaughanbiker/Development/liber/test/rsync/test_root/hash_log.csv");
#endif
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
HashTable<KeySize, Type>::~HashTable()
{
#if PROFILE == ENABLED
  mLog.close();
#endif
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
void HashTable<KeySize, Type>::insert(int key, Type value)
{
  mBuckets.at(hash(key)).push_back(std::make_pair(key, value));
  mnItemCount++;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::remove(int key, Type& value, Comparator& comp)
{
  std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator> location;
  Timestamp begin, end;

  begin.sample();
  location = get(key, value, comp);
  end.sample();

#if PROFILE == ENABLED
  mLog << key << "," << hash(key) << "," << mBuckets.at(hash(key)).size() << "," << (end - begin).fseconds() << "\n";
#endif

  if (location.first != std::string::npos)
  {
    mBuckets.at(location.first).erase(location.second);
    mnItemCount--;
  }

  return (location.first != std::string::npos);
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::find(int key, Type& item, Comparator& comp)
{
  std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator> location;
//  Timestamp begin, end;
//  begin.sample();
  location = get(key, item, comp);
//  end.sample();
//  mLog << key << "," << hash(key) << "," << mBuckets.at(hash(key)).size() << "," << (end - begin).fseconds() << "\n";
  return (location.first != std::string::npos);
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
void HashTable<KeySize, Type>::clear()
{
  for (size_t lnBucket = 0; lnBucket < mnBucketCount; lnBucket++)
  {
    mBuckets.at(lnBucket).clear();
  }

  mnItemCount = 0;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Destroyer>
void HashTable<KeySize, Type>::clear(Destroyer& destroy)
{
  for (size_t lnBucket = 0; lnBucket < mnBucketCount; lnBucket++)
  {
    typename HashTable<KeySize, Type>::Bucket& lrBucket = mBuckets.at(lnBucket);
    typename Bucket::iterator item = lrBucket.begin();

    for (; item != lrBucket.end(); ++item)
    {
      destroy(item->second);
    }

    lrBucket.clear();
  }

  mnItemCount = 0;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator>
HashTable<KeySize, Type>::get(int key, Type& value, Comparator& comp)
{
  size_t lnBucket = std::string::npos;

  typename HashTable<KeySize, Type>::Bucket& lrBucket = mBuckets.at(hash(key));
  typename Bucket::iterator item = lrBucket.begin();
  for (; item != lrBucket.end(); ++item)
  {
    if ((item->first == key) && comp(item->second))
    {
      value = item->second;
      lnBucket = hash(key);
      break;
    }
  }

  return std::make_pair(lnBucket, item);
}

}

#endif // HASH_TABLE_H
