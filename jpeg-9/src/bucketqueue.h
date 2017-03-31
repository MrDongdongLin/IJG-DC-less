#ifndef BUCKETQUEUE_H
#define BUCKETQUEUE_H

#include <assert.h>

using namespace std;

template< typename T >
struct BucketQueue {
  vector< vector< T > > buckets;
  int min_bucket;
  unsigned int pushes;
  unsigned int size_;
  BucketQueue( const unsigned int& b = 0 ) : buckets( b ), min_bucket( 0 ), pushes(0), size_( 0 ) {}
  
  inline void push( const T& t ) {
    assert( t.key >= 0 );
    if( size_== 0 || t.key < min_bucket ) min_bucket = t.key;
    //if( static_cast<unsigned int>( t.key ) >= buckets.size() ) buckets.resize( 2*t.key+1 );
    buckets[t.key].push_back( t );
    ++size_;
    ++pushes;
  }
  inline T top() const {
    assert( size_ > 0 );
    return buckets[min_bucket].back();
  }
  inline unsigned int min() const {
    return min_bucket;
  }
  inline void pop() {
    assert( size_ > 0 );
    assert( !buckets[min_bucket].empty() );
    buckets[min_bucket].pop_back();
    --size_;
    if( size_ > 0 ) {
      while( buckets[min_bucket].empty() ) ++min_bucket;
      assert( static_cast<unsigned int>( min_bucket ) < buckets.size() );
    }
  }
  inline void clear() {
    for( typename vector< vector< T > >::iterator iter = buckets.begin(), end = buckets.end(); iter != end; ++iter ) {
      iter->clear();
    }
    min_bucket = 0;
    size_ = 0;
  }
  inline bool empty() const {
    return size_ == 0;
  }
  inline unsigned int size() const {
    return size_;
  }
  
};

#endif
