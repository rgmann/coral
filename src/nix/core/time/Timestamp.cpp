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

using namespace coral;

//------------------------------------------------------------------------------
Timestamp::Timestamp()
   : seconds_(0)
   , nanoseconds_(0)
{
}

//------------------------------------------------------------------------------
Timestamp::Timestamp(const Timestamp& other)
   : seconds_(0)
   , nanoseconds_(0)
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

   clock_serv_t    cclock;
   mach_timespec_t mts;
   host_get_clock_service( mach_host_self(), CALENDAR_CLOCK, &cclock );
   clock_get_time( cclock, &mts );
   mach_port_deallocate( mach_task_self(), cclock );

   seconds_     = mts.tv_sec;
   nanoseconds_ = mts.tv_nsec;

#else

   struct timespec ts;
   clock_gettime( CLOCK_REALTIME, &ts );
   
   seconds_     = ts.tv_sec;
   nanoseconds_ = ts.tv_nsec;

#endif
}

//------------------------------------------------------------------------------
time_t Timestamp::seconds() const
{
   return seconds_;
}

//------------------------------------------------------------------------------
void Timestamp::seconds(time_t seconds)
{
   seconds_ = seconds;
}

//------------------------------------------------------------------------------
time_t* Timestamp::ptr()
{
   return &seconds_;
}

//------------------------------------------------------------------------------
ui32 Timestamp::nanoseconds() const
{
   return nanoseconds_;
}

//------------------------------------------------------------------------------
void Timestamp::nanoseconds(i32 ns)
{
   nanoseconds_ = ns;
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
   if ( this != &other )
   {
      seconds_     = other.seconds_;
      nanoseconds_ = other.nanoseconds_;
   }

   return *this;
}

//------------------------------------------------------------------------------
namespace coral {
Timestamp operator- ( const Timestamp& lhs, const Timestamp& rhs )
{
   Timestamp result;

   Timestamp left( lhs );
   Timestamp right( rhs );

   // Perform the carry for the later subtraction by updating y.
   if ( left.nanoseconds_ < right.nanoseconds_)
   {
      i32 nsec = ( right.nanoseconds_ - left.nanoseconds_) / NSEC_PER_SEC + 1;
      right.nanoseconds_ -= NSEC_PER_SEC * nsec;
      right.seconds_     += nsec;
   }

   if ( ( left.nanoseconds_ - right.nanoseconds_ ) > NSEC_PER_SEC )
   {
      i32 nsec = ( left.nanoseconds_ - right.nanoseconds_ ) / NSEC_PER_SEC;
      right.nanoseconds_ += NSEC_PER_SEC * nsec;
      right.seconds_     -= nsec;
   }
     
   // Compute the time remaining to wait. tv_usec is certainly positive.
   result.seconds_     = left.seconds_ - right.seconds_;
   result.nanoseconds_ = left.nanoseconds_ - right.nanoseconds_;

   return result;
}

//-----------------------------------------------------------------------------
Timestamp& Timestamp::operator -= (const Timestamp& other)
{
   *this = ( *this - other );
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
