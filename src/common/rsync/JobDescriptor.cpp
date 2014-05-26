#include "Log.h"
#include "JobDescriptor.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
JobDescriptor::JobDescriptor()
: mnSegmentSizeBytes(0)
, mbRemoteSource(false)
{
}

//-----------------------------------------------------------------------------
JobDescriptor::
JobDescriptor(const boost::filesystem::path& path, ui32 nSegmentSizeBytes)
: mnSegmentSizeBytes(nSegmentSizeBytes)
, mSourcePath(path)
, mbRemoteSource(false)
, mDestinationPath(path)
{
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
void JobDescriptor::setSource(const boost::filesystem::path& sourcePath, bool bRemote)
{
  mSourcePath = sourcePath;
  mbRemoteSource = bRemote;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getSource() const
{
  return mSourcePath;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::remote() const
{
  return mbRemoteSource;
}

//-----------------------------------------------------------------------------
void JobDescriptor::setDestination(const boost::filesystem::path& destinationPath)
{
  mDestinationPath = destinationPath;
}

//-----------------------------------------------------------------------------
const boost::filesystem::path& JobDescriptor::getDestination() const
{
  return mDestinationPath;
}

//-----------------------------------------------------------------------------
bool JobDescriptor::isValid() const
{
  return ((mDestinationPath.empty() == false) && (mSourcePath.empty() == false));
}

//-----------------------------------------------------------------------------
std::string JobDescriptor::serialize() const
{
  liber::netapp::PacketCtor ctor;

  if (isValid())
  {
    ctor.write(mnSegmentSizeBytes);
    ctor.writeCString(getSource().string());
    ctor.write(remote());
    ctor.writeCString(getDestination().string());
  }

  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
bool JobDescriptor::deserialize(const std::string& data)
{
  liber::netapp::PacketDtor dtor;
  dtor.setData(data);
  return deserialize(dtor);
}

//-----------------------------------------------------------------------------
bool JobDescriptor::deserialize(const char* pData, ui32 nSizeBytes)
{
  liber::netapp::PacketDtor dtor;
  dtor.stream.write(pData, nSizeBytes);
  return deserialize(dtor);
}

//-----------------------------------------------------------------------------
bool JobDescriptor::deserialize(liber::netapp::PacketDtor& dtor)
{
  if (!dtor.read(mnSegmentSizeBytes))
  {
    log::error("JobDescriptor::deserialize: "
               "Failed to deserialize segment size.\n");
    return false;
  }

  std::string lTempString;

  if (dtor.readCString(lTempString) == liber::netapp::PacketDtor::ReadFail)
  {
    log::error("JobDescriptor::deserialize: "
               "Failed to deserialize source path.\n");
    return false;
  }
  mSourcePath = lTempString;

  if (!dtor.read(mbRemoteSource))
  {
    log::error("JobDescriptor::deserialize: "
               "Failed to deserialize mbRemote.\n");
    return false;
  }

  if (dtor.readCString(lTempString) == liber::netapp::PacketDtor::ReadFail)
  {
    log::error("JobDescriptor::deserialize: "
               "Failed to deserialize destination path.\n");
    return false;
  }
  mDestinationPath = lTempString;

  return true;
}

