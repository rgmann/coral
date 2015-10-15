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



#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <time.h>
#include "BaseTypes.h"

namespace coral {

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

   f64  fseconds() const;

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

} // namespace coral

#endif // TIMESTAMP_H
