#ifndef RSYNC_JOB_DESCRIPTOR_H
#define RSYNC_JOB_DESCRIPTOR_H

#include <string>
#include <boost/filesystem.hpp>
#include "BaseTypes.h"
#include "PacketHelper.h"

namespace liber {
namespace rsync {

class JobDescriptor {
public:

  JobDescriptor();
  JobDescriptor(const boost::filesystem::path& path, ui32 nSegmentSizeBytes);

  void setSegmentSize(ui32 nSegmentSizeBytes);
  ui32 getSegmentSize() const;

  void setSource(const boost::filesystem::path&, bool bRemote = false);
  const boost::filesystem::path& getSource() const;
  bool remote() const;

  void setDestination(const boost::filesystem::path&);
  const boost::filesystem::path& getDestination() const;

  bool isValid() const;

  std::string serialize() const;

  bool deserialize(const std::string& data);
  bool deserialize(const char* pData, ui32 nSizeBytes);
  bool deserialize(liber::netapp::PacketDtor&);

private:

  ui32 mnSegmentSizeBytes;
  boost::filesystem::path mSourcePath;
  bool mbRemoteSource;
  boost::filesystem::path mDestinationPath;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_JOB_DESCRIPTOR_H
