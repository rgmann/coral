#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>
#include "BaseTypes.h"

namespace liber {

class Timestamp {
public:

  Timestamp();
  Timestamp(const Timestamp&);
  ~Timestamp();

  static Timestamp Now();

  void  sample();

  time_t seconds() const;

  void seconds(time_t seconds);

  time_t* ptr();

  f64   fseconds() const;

  i64  toMilliseconds() const;
  ui32 milliseconds() const;

  i64  toMicroseconds() const;
  ui32 microseconds() const;

  i64  toNanoseconds() const;
  ui32 nanoseconds() const;
  void nanoseconds(i32 ns);

  i64  diffInS(const Timestamp &start);

  i64  diffInMs(const Timestamp &start);

  i64  diffInUs(const Timestamp &start);

  i64  diffInNs(const Timestamp &start);

  Timestamp& operator= (const Timestamp&);

  friend Timestamp operator- (const Timestamp&, const Timestamp&);
  Timestamp& operator-= (const Timestamp&);

  friend Timestamp operator+ (const Timestamp&, const Timestamp&);
  Timestamp& operator+= (const Timestamp&);

  friend bool operator< (const Timestamp&, const Timestamp&);
  friend bool operator> (const Timestamp&, const Timestamp&);
  friend bool operator<= (const Timestamp&, const Timestamp&);
  friend bool operator>= (const Timestamp&, const Timestamp&);

private:
   
  time_t seconds_;
  i32    nanoseconds_;
};

} // namespace liber

#endif // TIMESTAMP_H
