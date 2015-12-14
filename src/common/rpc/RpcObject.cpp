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



#include <string>
#include <iostream>
#include <sstream>
#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "RpcObject.h"

using namespace coral;
using namespace coral::rpc;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
RpcObject::RpcObject()
{
  call_info_.uuid = boost::uuids::random_generator()();
}

//-----------------------------------------------------------------------------
RpcObject::RpcObject(const std::string &resourceName, 
                     const std::string &actionName)
{
   call_info_.resource = resourceName;
   call_info_.action   = actionName;
}

//-----------------------------------------------------------------------------
RpcObject::~RpcObject()
{
}

//-----------------------------------------------------------------------------
bool RpcObject::isValid() const
{
   return ( call_info_.resource.empty() == false ) &&
          ( call_info_.action.empty() == false );
}

//-----------------------------------------------------------------------------
RpcCallInfo& RpcObject::callInfo()
{
   return call_info_;
}

//-----------------------------------------------------------------------------
const RpcCallInfo& RpcObject::callInfo() const
{
   return call_info_;
}

//-----------------------------------------------------------------------------
RpcException& RpcObject::exception()
{
   exception_.call_info_ = callInfo();
   return exception_;
}

//-----------------------------------------------------------------------------
void RpcObject::setParams( const PbMessage& message )
{
   message.SerializeToString( &message_ );
}

//-----------------------------------------------------------------------------
void RpcObject::setParams( const std::string& message )
{
   message_.assign( message.data(), message.size() );
}

//-----------------------------------------------------------------------------
void RpcObject::getParams( PbMessage& message ) const
{
   message.ParseFromString( message_ );
}

//-----------------------------------------------------------------------------
void RpcObject::getParams(std::string& message) const
{
   message.assign( message_.data(), message_.size() );
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse( RpcObject& response ) const
{
   bool success = isValid();

   if ( success )
   {
      response.call_info_ = call_info_;
      response.exception_ = exception_;
   }

   return success;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(
   RpcObject&       response,
   const PbMessage& message ) const
{
   bool success = getResponse( response );

   if ( success )
   {
      response.setParams( message );
   }

   return success;
}

//-----------------------------------------------------------------------------
bool RpcObject::getResponse(
   RpcObject&         response,
   const std::string& message ) const
{
   bool success = getResponse( response );

   if ( success )
   {
      response.setParams( message );
   }

   return success;
}

//-----------------------------------------------------------------------------
void RpcObject::pack(coral::netapp::SerialStream& stream) const
{
   stream.write_string(callInfo().resource);
   stream.write_string(callInfo().action);

   stream.write((const char*)callInfo().uuid.data, callInfo().uuid.size());
   exception_.serialize(stream);
   stream.write(message_);
}

//-----------------------------------------------------------------------------
bool RpcObject::unpack(coral::netapp::SerialStream &stream)
{
   // Read the resource name
   if ( stream.read_string( callInfo().resource ) == SerialStream::ReadFail )
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   // Read the action name.
   if ( stream.read_string( callInfo().action ) == SerialStream::ReadFail )
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   if (stream.read((char*)callInfo().uuid.data, callInfo().uuid.size()) != SerialStream::ReadOk )
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   // Read the exception.
   if ( exception_.deserialize( stream ) == false )
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   // Read the protobuf message.
   if ( stream.read( message_ ) == SerialStream::ReadFail )
   {
      log::error("RpcObject::deserialize failure at %d\n", __LINE__);
      return false;
   }

   return true;
}

