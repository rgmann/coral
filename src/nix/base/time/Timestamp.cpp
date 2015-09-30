#include <string.h>
#include <sys/time.h>
#include <stdio.h>

#if defined(__APPLE__) || defined(__MACH__)
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#include "Timestamp.h"

#ifndef NSEC_PER_SEC
#define NSEC_PER_SEC       (1000000000)
#endif

using namespace liber;

//------------------------------------------------------------------------------
Timestamp::Timestamp()
: mSeconds(0)
, mNanoseconds(0)
{
}

//------------------------------------------------------------------------------
Timestamp::Timestamp(const Timestamp& other)
: mSeconds(0)
, mNanoseconds(0)
{
  *this = other;
}

//------------------------------------------------------------------------------
Timestamp::~Timestamp()
{
}

//------------------------------------------------------------------------------
Timestamp Timestamp::Now()
{
   Timestamp now;
   now.sample();
   return now;
}

//------------------------------------------------------------------------------
void Timestamp::sample()
{
#if defined(__APPLE__) && defined(__MACH__)
  clock_serv_t cclock;
  mach_timespec_t mts;
  //host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  mSeconds = mts.tv_sec;
  mNanoseconds = mts.tv_nsec;
#else
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  mSeconds = ts.tv_sec;
  mNanoseconds = ts.tv_nsec;
#endif
}

//------------------------------------------------------------------------------
time_t Timestamp::seconds() const
{
  return mSeconds;
}

//------------------------------------------------------------------------------
void Timestamp::seconds(time_t seconds)
{
  mSeconds = seconds;
}

//------------------------------------------------------------------------------
time_t* Timestamp::ptr()
{
  return &mSeconds;
}

//------------------------------------------------------------------------------
ui32 Timestamp::nanoseconds() const
{
  return mNanoseconds;
}

//------------------------------------------------------------------------------
void Timestamp::nanoseconds(i32 ns)
{
  mNanoseconds = ns;
}

//------------------------------------------------------------------------------
ui32 Timestamp::microseconds() const
{
  return nanoseconds() / 1000;
}

//------------------------------------------------------------------------------
ui32 Timestamp::milliseconds() const
{
  return microseconds() / 1000;
}

//------------------------------------------------------------------------------
i64 Timestamp::toNanoseconds() const
{
  return (i64)seconds() * 1000000000 + (i64)nanoseconds();
}

//------------------------------------------------------------------------------
i64 Timestamp::toMicroseconds() const
{
  return toNanoseconds() / 1000;
}

//------------------------------------------------------------------------------
i64 Timestamp::toMilliseconds() const
{
  return toMicroseconds() / 1000;
}

//------------------------------------------------------------------------------
f64 Timestamp::fseconds() const
{
   return (f64)seconds() + ((f64)nanoseconds()) / 1000000000.0;
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInS(const Timestamp &start)
{
   return (seconds() - start.seconds());
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInNs(const Timestamp &start)
{
  return (toNanoseconds() - start.toNanoseconds());
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInUs(const Timestamp &start)
{
  return (toMicroseconds() - start.toMicroseconds());
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInMs(const Timestamp &start)
{
  return (toMilliseconds() - start.toMilliseconds());
}

//------------------------------------------------------------------------------
Timestamp& Timestamp::operator= (const Timestamp& other)
{
  if (this != &other)
  {
    //memcpy(mpTimestamp, other.mpTimestamp, sizeof(TimeCore));
    mSeconds = other.mSeconds;
    mNanoseconds = other.mNanoseconds;
  }

  return *this;
}

//------------------------------------------------------------------------------
namespace liber {
Timestamp operator- (const Timestamp& lhs, const Timestamp& rhs)
{
  Timestamp result;

  Timestamp x = lhs;
  Timestamp y = rhs;

  // Perform the carry for the later subtraction by updating y.
  if (x.mNanoseconds < y.mNanoseconds)
  {
    int nsec = (y.mNanoseconds - x.mNanoseconds) / NSEC_PER_SEC + 1;
    y.mNanoseconds -= NSEC_PER_SEC * nsec;
    y.mSeconds += nsec;
  }
  if (x.mNanoseconds - y.mNanoseconds > NSEC_PER_SEC)
  {
    int nsec = (x.mNanoseconds - y.mNanoseconds) / NSEC_PER_SEC;
    y.mNanoseconds += NSEC_PER_SEC * nsec;
    y.mSeconds -= nsec;
  }
     
  // Compute the time remaining to wait. tv_usec is certainly positive.
  result.mSeconds = x.mSeconds - y.mSeconds;
  result.mNanoseconds = x.mNanoseconds - y.mNanoseconds;

  return result;
}

//-----------------------------------------------------------------------------
Timestamp& Timestamp::operator -= (const Timestamp& other)
{
  *this = (*this - other);
  return *this;
}

//-----------------------------------------------------------------------------
Timestamp operator+ (const Timestamp& lhs, const Timestamp& rhs)
{
  Timestamp result;
  Timestamp zero;

  result = lhs - (zero - rhs);

  return result;
}

//-----------------------------------------------------------------------------
Timestamp& Timestamp::operator += (const Timestamp& other)
{
  *this = (*this + other);
  return *this;
}

//-----------------------------------------------------------------------------
bool operator< (const Timestamp& lhs, const Timestamp& rhs)
{
  return (lhs.toNanoseconds() < rhs.toNanoseconds());
}

//-----------------------------------------------------------------------------
bool operator> (const Timestamp& lhs, const Timestamp& rhs)
{
  return (lhs.toNanoseconds() > rhs.toNanoseconds());
}

//-----------------------------------------------------------------------------
bool operator<= (const Timestamp& lhs, const Timestamp& rhs)
{
  return (lhs.toNanoseconds() <= rhs.toNanoseconds());
}

//-----------------------------------------------------------------------------
bool operator>= (const Timestamp& lhs, const Timestamp& rhs)
{
  return (lhs.toNanoseconds() >= rhs.toNanoseconds());
}
}
