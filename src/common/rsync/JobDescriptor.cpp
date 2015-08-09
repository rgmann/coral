#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "JobDescriptor.h"

using namespace liber;
using namespace liber::rsync;
using namespace liber::netapp;


//-----------------------------------------------------------------------------
JobDescriptor::JobDescriptor()
// : segment_size_bytes_(0)
// , maximum_chunk_size_( 1024 )
: remotely_requested_(false)
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
  limits_.segment_size_bytes = segment_size_bytes;
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::getSegmentSize() const
{
  return limits_.segment_size_bytes;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setLimits( const JobLimits& limits )
{
  limits_ = limits;
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::completionTimeoutMs() const
{
  return limits_.completion_timeout_ms;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSource(
  const boost::filesystem::path& source_path,
  bool remote
)
{
  source_path_.setPath( source_path );
  source_path_.setRemote( remote );
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSource( const ResourcePath& path )
{
  source_path_ = path;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getSourcePath() const
{
  return source_path_.path();
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
  destination_path_.setPath( path );
  destination_path_.setRemote( remote );
  remotely_requested_ = destination_path_.remote();
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getDestinationPath() const
{
  return destination_path_.path();
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
    ( destination_path_.path().empty() == false ) &&
    ( source_path_.path().empty() == false )
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
  return limits_.maximum_chunk_size_bytes;
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::SerialStream& ctor)
{
  const_cast<const JobDescriptor*>(this)->pack(ctor);
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::SerialStream& ctor) const
{
  if ( isValid() )
  {
    ctor.write( limits_.segment_size_bytes );
    ctor.write( limits_.maximum_chunk_size_bytes );
    ctor.write( limits_.completion_timeout_ms );
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
  if (dtor.read( limits_.segment_size_bytes ) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize segment size.\n");
    return false;
  }

  if (dtor.read( limits_.maximum_chunk_size_bytes ) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize maximum chunk size.\n");
    return false;
  }

  if (dtor.read( limits_.completion_timeout_ms ) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize completion_timeout_ms size.\n");
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

