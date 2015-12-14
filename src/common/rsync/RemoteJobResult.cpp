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



#include <boost/uuid/nil_generator.hpp>
#include "Log.h"
#include "RemoteJobResult.h"

using namespace coral;
using namespace coral::netapp;
using namespace coral::rsync;

//----------------------------------------------------------------------------
RemoteJobResult::RemoteJobResult()
: Serializable()
{
  mUUID = boost::uuids::nil_uuid();
}

//----------------------------------------------------------------------------
RemoteJobResult::RemoteJobResult(const boost::uuids::uuid& rID, const JobReport& rReport)
{
  mUUID = rID;
  mReport = rReport;
}

//----------------------------------------------------------------------------
boost::uuids::uuid& RemoteJobResult::uuid()
{
  return mUUID;
}

//----------------------------------------------------------------------------
JobReport& RemoteJobResult::report()
{
  return mReport;
}

//----------------------------------------------------------------------------
void RemoteJobResult::pack(SerialStream& ctor) const
{
  ctor.write((const char*)mUUID.data, mUUID.size());
  mReport.serialize(ctor);
}

//----------------------------------------------------------------------------
bool RemoteJobResult::unpack(SerialStream& dtor)
{
  if (dtor.read((char*)mUUID.data, mUUID.size()) != coral::netapp::SerialStream::ReadOk)
  {
    log::error("RemoteJobResult::unpack - failed to deserialize UUID\n");
    return false;
  }

  if (mReport.deserialize(dtor) == false)
  {
    return false;
  }

  return true;
}


