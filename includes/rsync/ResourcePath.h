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



#ifndef  RESOURCE_PATH_H
#define  RESOURCE_PATH_H


#include <string>
#include <boost/filesystem.hpp>
#include "BaseTypes.h"
#include "Serializable.h"


namespace coral {
namespace rsync {


class ResourcePath : public coral::netapp::Serializable {
public:

   ResourcePath();
   ResourcePath( const boost::filesystem::path&, bool remote );
   virtual ~ResourcePath() {};

   const boost::filesystem::path& path() const;
   void setPath( const boost::filesystem::path& path );

   bool remote() const;
   void setRemote( bool remote );

protected:

   void pack(coral::netapp::SerialStream& rCtor);
   void pack(coral::netapp::SerialStream& rCtor) const;
   bool unpack(coral::netapp::SerialStream& rDtor);

private:

   boost::filesystem::path path_;
   bool remote_;
};

class RemoteResourcePath : public ResourcePath {
public:

   RemoteResourcePath( const boost::filesystem::path& path ) :
      ResourcePath( path, true ) {};

};

class LocalResourcePath : public ResourcePath {
public:

   LocalResourcePath( const boost::filesystem::path& path ) :
      ResourcePath( path, false ) {};

};

} // namespace rsync
} // namespace coral

#endif // RESOURCE_PATH_H
