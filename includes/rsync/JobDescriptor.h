#ifndef RSYNC_JOB_DESCRIPTOR_H
#define RSYNC_JOB_DESCRIPTOR_H

#include <string>
#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include "BaseTypes.h"
#include "Serializable.h"

namespace liber {
namespace rsync {


class ResourcePath : public liber::netapp::Serializable {
public:

  ResourcePath();
  ResourcePath(const boost::filesystem::path&, bool bRemote = false);

  boost::filesystem::path path;
  bool remote;

protected:

  void pack(liber::netapp::SerialStream& rCtor);
  void pack(liber::netapp::SerialStream& rCtor) const;
  bool unpack(liber::netapp::SerialStream& rDtor);
};


class JobDescriptor : public liber::netapp::Serializable {
public:

  JobDescriptor();

  void  setRemoteRequest();
  bool  isRemoteRequest() const;

  void  setSegmentSize(ui32 nSegmentSizeBytes);
  ui32  getSegmentSize() const;

  void  setSource(const boost::filesystem::path&, bool bRemote = false);
  void  setSource(const ResourcePath& path);
  const boost::filesystem::path& getSourcePath() const;
  const ResourcePath& getSource() const;
  ResourcePath& getSource();

  void  setDestination(const boost::filesystem::path&, bool bRemote = false);
  void  setDestination(const ResourcePath& path);
  const boost::filesystem::path& getDestinationPath() const;
  const ResourcePath& getDestination() const;
  ResourcePath& getDestination();

  bool  isValid() const;

  const boost::uuids::uuid& uuid() const; 

protected:

  void  pack(liber::netapp::SerialStream& rCtor);
  void  pack(liber::netapp::SerialStream& rCtor) const;
  bool  unpack(liber::netapp::SerialStream& rDtor);

private:

  ui32 mnSegmentSizeBytes;

  ResourcePath mSource;
  ResourcePath mDestination;

  bool mbRemotelyRequested;

  boost::uuids::uuid mUUID;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_DESCRIPTOR_H
