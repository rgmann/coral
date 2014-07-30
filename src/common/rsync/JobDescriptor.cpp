#include <boost/uuid/random_generator.hpp>
#include "Log.h"
#include "JobDescriptor.h"

using namespace liber;
using namespace liber::rsync;


//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
ResourcePath::ResourcePath()
: remote(false)
{
}

//-----------------------------------------------------------------------------
ResourcePath::ResourcePath(const boost::filesystem::path& rPath, bool bRemote)
: path(rPath)
, remote(bRemote)
{
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(liber::netapp::PacketCtor& rCtor)
{
  rCtor.writeCString(path.string());
  rCtor.write(remote);
}

//-----------------------------------------------------------------------------
void ResourcePath::pack(liber::netapp::PacketCtor& rCtor) const
{
  rCtor.writeCString(path.string());
  rCtor.write(remote);
}

//-----------------------------------------------------------------------------
bool ResourcePath::unpack(liber::netapp::PacketDtor& rDtor)
{
  std::string lTempString;
  if (rDtor.readCString(lTempString) != liber::netapp::PacketDtor::ReadOk)
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
: mnSegmentSizeBytes(0)
, mbRemotelyRequested(false)
, mUUID(boost::uuids::random_generator()())
{
}

//-----------------------------------------------------------------------------
void JobDescriptor::setRemoteRequest()
{
  mbRemotelyRequested = true;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::isRemoteRequest() const
{
  return mbRemotelyRequested;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSegmentSize(ui32 nSegmentSizeBytes)
{
  mnSegmentSizeBytes = nSegmentSizeBytes;
}

//-----------------------------------------------------------------------------
ui32 JobDescriptor::getSegmentSize() const
{
  return mnSegmentSizeBytes;
}

//-----------------------------------------------------------------------------
void JobDescriptor::
setSource(const boost::filesystem::path& sourcePath, bool bRemote)
{
  mSource.path = sourcePath;
  mSource.remote = bRemote;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setSource(const ResourcePath& path)
{
  mSource = path;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getSourcePath() const
{
  return mSource.path;
}

//-----------------------------------------------------------------------------
const ResourcePath& JobDescriptor::getSource() const
{
  return mSource;
}

//-----------------------------------------------------------------------------
ResourcePath& JobDescriptor::getSource()
{
  return mSource;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setDestination(const ResourcePath& destination)
{
  mDestination = destination;
  //mbRemotelyRequested = mDestination.remote;
}

//-----------------------------------------------------------------------------
void JobDescriptor::
setDestination(const boost::filesystem::path& path, bool bRemote)
{
  mDestination.path = path;
  mDestination.remote = bRemote;
  mbRemotelyRequested = mDestination.remote;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getDestinationPath() const
{
  return mDestination.path;
}

//-----------------------------------------------------------------------------
const ResourcePath& JobDescriptor::getDestination() const
{
  return mDestination;
}

//-----------------------------------------------------------------------------
ResourcePath& JobDescriptor::getDestination()
{
  return mDestination;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::isValid() const
{
  return ((mDestination.path.empty() == false) && (mSource.path.empty() == false));
}

//-----------------------------------------------------------------------------
const boost::uuids::uuid& JobDescriptor::uuid() const
{
  return mUUID;
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::PacketCtor& ctor)
{
  const_cast<const JobDescriptor*>(this)->pack(ctor);
}

//-----------------------------------------------------------------------------
void JobDescriptor::pack(liber::netapp::PacketCtor& ctor) const
{
  if (isValid())
  {
    ctor.write(mnSegmentSizeBytes);
    ctor.write((const char*)mUUID.data, mUUID.size());
    mSource.serialize(ctor);
    mDestination.serialize(ctor);
  }
  else
  {
    log::error("JobDescriptor::pack - Invalid descriptor not serialized\n");
  }
}

//-----------------------------------------------------------------------------
bool JobDescriptor::unpack(liber::netapp::PacketDtor& dtor)
{
  if (dtor.read(mnSegmentSizeBytes) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize segment size.\n");
    return false;
  }

  if (dtor.read((char*)mUUID.data, mUUID.size()) != liber::netapp::PacketDtor::ReadOk)
  {
    log::error("RemoteJobStatus::unpack - failed to deserialize UUID\n");
    return false;
  }

  if (mSource.deserialize(dtor) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize source path.\n");
    return false;
  }

  if (mDestination.deserialize(dtor) == false)
  {
    log::error("JobDescriptor::unpack: "
               "Failed to deserialize mbRemote.\n");
    return false;
  }

  return true;
}

