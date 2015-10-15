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
#include "ResourcePath.h"

using namespace coral;
using namespace coral::rsync;
using namespace coral::netapp;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResourcePath::ResourcePath()
: remote_( false )
{
}

//-----------------------------------------------------------------------------
ResourcePath::ResourcePath( const boost::filesystem::path& path, bool remote )
: path_  ( path )
, remote_( remote )
{
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& ResourcePath::path() const
{
   return path_;
}

//-----------------------------------------------------------------------------
void ResourcePath::setPath( const boost::filesystem::path& path )
{
   path_ = path;
}

//-----------------------------------------------------------------------------
bool ResourcePath::remote() const
{
   return remote_;
}

//-----------------------------------------------------------------------------
void ResourcePath::setRemote( bool remote )
{
   remote_ = remote;
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(coral::netapp::SerialStream& rCtor)
{
  rCtor.writeCString( path_.string() );
  rCtor.write( remote_ );
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(coral::netapp::SerialStream& rCtor) const
{
  rCtor.writeCString( path_.string() );
  rCtor.write( remote_ );
}

//-----------------------------------------------------------------------------
bool ResourcePath::unpack(coral::netapp::SerialStream& rDtor)
{
  std::string path_string;
  if (rDtor.readCString(path_string) != SerialStream::ReadOk)
  {
    log::error("ResourcePath::unpack - Failed to read 'path'\n");
    return false;
  }
  path_ = path_string;

  if (rDtor.read( remote_ ) == false)
  {
    log::error("ResourcePath::unpack - Failed to read 'remote'\n");
    return false;
  }

  return true;
}
