#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <cmath>
#include <vector>
#include <list>
#include <utility>
#include <string>
#include <fstream>
#include "Timestamp.h"


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
  inline size_t size() const { return item_count_; };

  inline size_t buckets() const { return bucket_count_; };

  void histogram( std::ostream& stream );

private:

  typedef std::pair<int, Type> Item;
  typedef std::list<Item> Bucket;

  /**
   * Extremely simple hash function.
   */
  size_t hash(int key) { return (size_t)(key % bucket_count_); };

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


private:

  // Hash table bucket count.
  size_t bucket_count_;

  // The hash table is a vector of vectors.
  std::vector<Bucket> buckets_;

  // Number of items currently stored in the hash table.
  size_t item_count_;
};

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
HashTable<KeySize, Type>::HashTable()
: item_count_(0)
{
  bucket_count_ = (int)std::pow(2, KeySize);

  for (int nBucket = 0; nBucket < bucket_count_; nBucket++)
  {
    buckets_.push_back(Bucket());
  }
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
HashTable<KeySize, Type>::~HashTable()
{
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
void HashTable<KeySize, Type>::insert(int key, Type value)
{
  buckets_.at( hash( key ) ).push_back( std::make_pair( key, value ) );
  item_count_++;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::remove(int key, Type& value, Comparator& comp)
{
  std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator> location;

  location = get(key, value, comp);

  if ( location.first != std::string::npos )
  {
    buckets_.at( location.first ).erase( location.second );
    item_count_--;
  }

  return (location.first != std::string::npos);
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::find(int key, Type& item, Comparator& comp)
{
  std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator> location;

  location = get(key, item, comp);

  return (location.first != std::string::npos);
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
void HashTable<KeySize, Type>::clear()
{
  for (size_t bucket_index = 0; bucket_index < bucket_count_; bucket_index++)
  {
    buckets_.at( bucket_index ).clear();
  }

  item_count_ = 0;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Destroyer>
void HashTable<KeySize, Type>::clear(Destroyer& destroy)
{
  for (size_t bucket_index = 0; bucket_index < bucket_count_; bucket_index++)
  {
    typename HashTable<KeySize, Type>::Bucket& bucket = buckets_.at(bucket_index);
    typename Bucket::iterator item = bucket.begin();

    for (; item != bucket.end(); ++item)
    {
      destroy( item->second );
    }

    bucket.clear();
  }

  item_count_ = 0;
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type> template<typename Comparator>
std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator>
HashTable<KeySize, Type>::get(int key, Type& value, Comparator& comp)
{
  size_t hashed_key   = hash(key);
  size_t bucket_index = std::string::npos;

  typename HashTable<KeySize, Type>::Bucket& bucket = buckets_.at( hashed_key );
  typename Bucket::iterator item = bucket.begin();
  for (; item != bucket.end(); ++item)
  {
    if ( item->first == key )
    {
      if ( comp( item->second ) )
      {
        value = item->second;
        bucket_index = hashed_key;
        break;
      }
    }
  }

  return std::make_pair( bucket_index, item );
}

//----------------------------------------------------------------------------
template<int KeySize, typename Type>
void HashTable<KeySize, Type>::histogram( std::ostream& stream )
{
  typename std::vector<Bucket>::iterator bucket_iterator = buckets_.begin();
  size_t bucket_index = 0;
  for ( ; bucket_iterator != buckets_.end(); bucket_iterator++ )
  {
    stream << bucket_index++ << "," << bucket_iterator->size() << "\n";
  }
}

}

#endif // HASH_TABLE_H