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



#include "Log.h"
#include "RsyncQueryResponse.h"

using namespace coral::netapp;
using namespace coral::rsync;

//-----------------------------------------------------------------------------
RsyncQueryResponse::RsyncQueryResponse()
:  status_( kRsyncCommError )
{
}

//-----------------------------------------------------------------------------
RsyncQueryResponse::RsyncQueryResponse(
  const boost::uuids::uuid& uuid,
  RsyncError               status )
: uuid_( uuid )
, status_( status )
{
}

//-----------------------------------------------------------------------------
const boost::uuids::uuid& RsyncQueryResponse::uuid() const
{
  return uuid_;
}

//-----------------------------------------------------------------------------
RsyncError RsyncQueryResponse::status() const
{
  return status_;
}

//-----------------------------------------------------------------------------
void RsyncQueryResponse::pack(coral::netapp::SerialStream& ctor) const
{
  ctor.write( (ui32)status_ );
  ctor.write((const char*)uuid_.data, uuid_.size());
}

//-----------------------------------------------------------------------------
bool RsyncQueryResponse::unpack(coral::netapp::SerialStream& dtor)
{
  ui32 temp_status = kRsyncCommError;
  if ( dtor.read( temp_status ) == false )
  {
    coral::log::error(
      "RsyncQueryResponse::unpack: Failed to deserialize segment size.\n");
    return false;
  }
  status_ = (RsyncError)temp_status;

  if ( dtor.read( (char*)uuid_.data, uuid_.size() ) != SerialStream::ReadOk )
  {
    coral::log::error( "RsyncQueryResponse::unpack - failed to deserialize UUID\n" );
    return false;
  }

  return true;
}
