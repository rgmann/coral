#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "JobDescriptor.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResourcePath::ResourcePath()
: remote(false)
{
}

//-----------------------------------------------------------------------------
ResourcePath::ResourcePath( const boost::filesystem::path& path, bool remote )
: path(path)
, remote(remote)
{
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(liber::netapp::SerialStream& rCtor)
{
  rCtor.writeCString(path.string());
  rCtor.write(remote);
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(liber::netapp::SerialStream& rCtor) const
{
  rCtor.writeCString(path.string());
  rCtor.write(remote);
}

//-----------------------------------------------------------------------------
bool ResourcePath::unpack(liber::netapp::SerialStream& rDtor)
{
  std::string lTempString;
  if (rDtor.readCString(lTempString) != SerialStream::ReadOk)
  {
    log::error("ResourcePath::unpack - Failed to read 'path'\n");
    return false;
  }
  path = lTempString;

  if (rDtor.read(remote) == false)
  {
    log::error("ResourcePath::unpack - Failed to read 'remote'\n");
    return false;
  }

  return true;
}


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JobDescriptor::JobDescriptor()
: segment_size_bytes_(0)
, maximum_chunk_size_( 1024 )
, remotely_requested_(false)
, uuid_(boost::uuids::random_generator()())
{
}

//-----------------------------------------------------------------------------
void JobDescriptor::setRemoteRequest()
{
  remotely_requested_ = true;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::isRemoteRequest() const
{
  return remotely_requested_;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSegmentSize( ui32 segment_size_bytes )
{
  segment_size_bytes_ = segment_size_bytes;
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::getSegmentSize() const
{
  return segment_size_bytes_;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSource(
  const boost::filesystem::path& source_path,
  bool remote
)
{
  source_path_.path   = source_path;
  source_path_.remote = remote;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSource(const ResourcePath& path)
{
  source_path_ = path;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getSourcePath() const
{
  return source_path_.path;
}

//-----------------------------------------------------------------------------
const ResourcePath& JobDescriptor::getSource() const
{
  return source_path_;
}

//-----------------------------------------------------------------------------
ResourcePath& JobDescriptor::getSource()
{
  return source_path_;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setDestination(const ResourcePath& destination)
{
  destination_path_ = destination;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setDestination(
  const boost::filesystem::path& path,
  bool remote
)
{
  destination_path_.path = path;
  destination_path_.remote = remote;
  remotely_requested_ = destination_path_.remote;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getDestinationPath() const
{
  return destination_path_.path;
}

//-----------------------------------------------------------------------------
const ResourcePath& JobDescriptor::getDestination() const
{
  return destination_path_;
}

//-----------------------------------------------------------------------------
ResourcePath& JobDescriptor::getDestination()
{
  return destination_path_;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::isValid() const
{
  return (
    ( destination_path_.path.empty() == false ) &&
    ( source_path_.path.empty() == false )
  );
}

//-----------------------------------------------------------------------------
const boost::uuids::uuid& JobDescriptor::uuid() const
{
  return uuid_;
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::getMaximumChunkSize() const
{
  return maximum_chunk_size_;
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::SerialStream& ctor)
{
  const_cast<const JobDescriptor*>(this)->pack(ctor);
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::SerialStream& ctor) const
{
  if (isValid())
  {
    ctor.write(segment_size_bytes_);
    ctor.write((const char*)uuid_.data, uuid_.size());
    source_path_.serialize(ctor);
    destination_path_.serialize(ctor);
  }
  else
  {
    log::error("JobDescriptor::pack - Invalid descriptor not serialized\n");
  }
}

//-----------------------------------------------------------------------------
bool JobDescriptor::unpack(liber::netapp::SerialStream& dtor)
{
  if (dtor.read(segment_size_bytes_) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize segment size.\n");
    return false;
  }

  if (dtor.read((char*)uuid_.data, uuid_.size()) != SerialStream::ReadOk)
  {
    log::error("RemoteJobStatus::unpack - failed to deserialize UUID\n");
    return false;
  }

  if (source_path_.deserialize(dtor) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize source path.\n");
    return false;
  }

  if (destination_path_.deserialize(dtor) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize mbRemote.\n");
    return false;
  }

  return true;
}

