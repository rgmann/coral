#ifndef SEM_H
#define SEM_H

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include "BaseTypes.h"

namespace liber {
namespace thread {

class Semaphore {
public:
   
  enum SemStatus
  {
    SemOk,
    SemAcquired,
    SemTimeout,
    SemError
  };

  enum SemTimeout
  {
    SemWaitForever = -1
  };

  enum { kUnlimited = -1 };

  Semaphore( i32 start, i32 max = kUnlimited );
  virtual ~Semaphore();

  SemStatus take( int timeout_ms = SemWaitForever );

  SemStatus give();
   
private:

  i32 maximum_count_;

  i32 count_;

  boost::mutex lock_;
  boost::condition_variable condition_;
};

}
}

#endif // SEM_H
