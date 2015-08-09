#include "Log.h"
#include "ResourcePath.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

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
void ResourcePath::pack(liber::netapp::SerialStream& rCtor)
{
  rCtor.writeCString( path_.string() );
  rCtor.write( remote_ );
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(liber::netapp::SerialStream& rCtor) const
{
  rCtor.writeCString( path_.string() );
  rCtor.write( remote_ );
}

//-----------------------------------------------------------------------------
bool ResourcePath::unpack(liber::netapp::SerialStream& rDtor)
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
