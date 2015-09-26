#include <iostream>
#include "Log.h"
#include "RpcMarshalledCall.h"

using namespace liber::rpc;
using namespace liber::thread;


//-----------------------------------------------------------------------------
RpcMarshalledCall::RpcMarshalledCall( const RpcObject &object )
   : is_disposed_( false )
   , cancelled_( false )
{
   mParamObj = object;
}

//-----------------------------------------------------------------------------
RpcPacket* RpcMarshalledCall::getRpcPacket() const
{
   return new (std::nothrow) RpcPacket(mParamObj);
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::getResult(RpcObject &result)
{
   result = mResultObj;
}

//-----------------------------------------------------------------------------
const RpcObject& RpcMarshalledCall::input() const
{
   return mParamObj;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::dispose()
{
   is_disposed_ = true;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::isDisposed() const
{
   return is_disposed_;
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::notify(const RpcObject &object)
{
   mResultObj = object;
   liber::log::status("RpcMarshalledCall::notify: %s\n", mResultObj.exception().toString().c_str());
   mSem.give();
}

//-----------------------------------------------------------------------------
void RpcMarshalledCall::cancel()
{
   cancelled_ = true;
   mSem.give();
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::cancelled() const
{
  return cancelled_;
}

//-----------------------------------------------------------------------------
bool RpcMarshalledCall::wait( ui32 timeout_ms, bool cancel_on_timeout )
{
   bool success = ( mSem.take( timeout_ms ) == Semaphore::SemAcquired );

   if ( success && cancel_on_timeout )
   {
     is_disposed_ = true;
   }

   return success;
}


