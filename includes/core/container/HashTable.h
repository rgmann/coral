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


namespace coral {

template<size_t KeySize, typename Type>
class HashTable {
public:

  HashTable();
  ~HashTable();

  ///
  /// Insert an item.
  ///
  /// @param  key  Item key
  /// @param  value Item value
  /// @return void
  ///
  void insert( size_t key, Type value);

  ///
  /// Remove an item by key and comparator.
  ///
  /// @param  key  Item key
  /// @param  value  Item (if found)
  /// @param  comp  Comparator functor
  ///
  template<typename Comparator>
  bool remove(size_t key, Type& value, Comparator& comp);

  ///
  /// Retrieve an item by key and comparator.
  ///
  /// @param  key  Item key
  /// @param  item Item (if found)
  /// @param  comp Comparator functor
  ///
  template<typename Comparator>
  bool find(size_t key, Type& item, Comparator& comp);

  void clear();

  template<typename Destroyer>
  void clear(Destroyer& destroy);

  ///
  /// Get the number of items stored in the HashTable.
  ///
  /// @return  size_t  Number of items stored
  ///
  inline size_t size() const { return item_count_; };

  ///
  /// Number of "buckets" in the HashTable. The number of buckets, B, is determined
  /// by the keysize, K, as B = 2^K.
  ///
  /// @return size_t Number of buckets
  ///
  inline size_t buckets() const { return bucket_count_; };

  ///
  /// Dump a comma-delimited histogram of this HashTable's utility to the
  /// specified output stream. Each line specifies the bucket index in the
  /// first column and the number of items in the bucket in the second column.
  ///
  /// @param  stream  Output stream
  /// @return void
  ///
  void histogram( std::ostream& stream );

private:

  typedef std::pair<size_t, Type> Item;
  typedef std::list<Item> Bucket;

  ///
  /// Extremely simple hash function.
  ///
  /// TODO: Consider a more sophisticated hash function to more
  /// evenly distribute elements to buckets.
  ///
  size_t hash(size_t key) { return (size_t)( key % bucket_count_ ); };

  /**
   * Access an item by item by key and comparison functor.
   *
   * @param key[size_t]    - Hash table key
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
  std::pair<size_t, typename Bucket::iterator> get(size_t, Type& value, Comparator&);


private:

  // Hash table bucket count.
  size_t bucket_count_;

  // The hash table is a vector of vectors.
  std::vector<Bucket> buckets_;

  // Number of items currently stored in the hash table.
  size_t item_count_;
};

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type>
HashTable<KeySize, Type>::HashTable()
: item_count_(0)
{
  bucket_count_ = (size_t)std::pow(2, KeySize);

  for (size_t nBucket = 0; nBucket < bucket_count_; nBucket++)
  {
    buckets_.push_back(Bucket());
  }
}

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type>
HashTable<KeySize, Type>::~HashTable()
{
}

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type>
void HashTable<KeySize, Type>::insert(size_t key, Type value)
{
  buckets_.at( hash( key ) ).push_back( std::make_pair( key, value ) );
  item_count_++;
}

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::remove(size_t key, Type& value, Comparator& comp)
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
template<size_t KeySize, typename Type> template<typename Comparator>
bool HashTable<KeySize, Type>::find(size_t key, Type& item, Comparator& comp)
{
  std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator> location;

  location = get(key, item, comp);

  return (location.first != std::string::npos);
}

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type>
void HashTable<KeySize, Type>::clear()
{
  for (size_t bucket_index = 0; bucket_index < bucket_count_; bucket_index++)
  {
    buckets_.at( bucket_index ).clear();
  }

  item_count_ = 0;
}

//----------------------------------------------------------------------------
template<size_t KeySize, typename Type> template<typename Destroyer>
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
template<size_t KeySize, typename Type> template<typename Comparator>
std::pair<size_t, typename HashTable<KeySize, Type>::Bucket::iterator>
HashTable<KeySize, Type>::get(size_t key, Type& value, Comparator& comp)
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
template<size_t KeySize, typename Type>
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
