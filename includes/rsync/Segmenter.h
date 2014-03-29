#ifndef RSYNC_SEGMENTER_H
#define RSYNC_SEGMENTER_H

#include <fstream>
#include <boost/filesystem.hpp>
#include "SegmentReceiver.h"

namespace liber {
namespace rsync {


class JobDescriptor {
public:

  JobDescriptor();
  JobDescriptor(const boost::filesystem::path& path, ui32 nSegmentSizeBytes);

  ui32 getSegmentSize() const;
  boost::filesystem::path& getPath();
  const boost::filesystem::path& getPath() const;
  bool isValid() const;

private:

  ui32 mnSegmentSizeBytes;
  boost::filesystem::path mPath;
};


class Segmenter {
public:

  enum SegmentDistance {

    // The requester does not overlap.
    FullStride,

    // The Authority creates a segment for every offset.
    EveryOffset
  };

  Segmenter();
  Segmenter(ui32 nSegmentSizeBytes, SegmentDistance distance = FullStride);

  bool process(std::istream& istream, liber::rsync::SegmentReceiver& rReceiver);

private:

  ui32 mnSegmentSizeBytes;
  SegmentDistance mDistance;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENTER_H
