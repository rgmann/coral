#include <string.h>
#include "Timestamp.h"

//------------------------------------------------------------------------------
Timestamp::Timestamp()
{
   memset(&mTV, 0, sizeof(mTV));
}

//------------------------------------------------------------------------------
Timestamp::~Timestamp()
{
}

//------------------------------------------------------------------------------
void Timestamp::sample()
{
   gettimeofday(&mTV, NULL);
}

//------------------------------------------------------------------------------
time_t Timestamp::seconds()
{
   return mTV.tv_sec;
}

//------------------------------------------------------------------------------
void Timestamp::seconds(time_t seconds)
{
   mTV.tv_sec = seconds;
}

//------------------------------------------------------------------------------
ui64 Timestamp::microseconds()
{
   return mTV.tv_usec;
}

//------------------------------------------------------------------------------
time_t* Timestamp::ptr()
{
   return &mTV.tv_sec;
}

//------------------------------------------------------------------------------
f32 Timestamp::fseconds()
{
   return (f32)(mTV.tv_usec/1000000) + ((f32)mTV.tv_usec) / 1000000.0;
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInS(const Timestamp &start)
{
   return (mTV.tv_sec - start.mTV.tv_sec);
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInMs(const Timestamp &start)
{
   return (mTV.tv_sec - start.mTV.tv_sec) * 1000;
}

//------------------------------------------------------------------------------
i64 Timestamp::diffInUs(const Timestamp &start)
{
   return ((mTV.tv_sec - start.mTV.tv_sec) * 1000000) + 
            (mTV.tv_usec - start.mTV.tv_usec);
}

