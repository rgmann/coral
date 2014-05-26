#ifndef RSYNC_SEGMENTER_H
#define RSYNC_SEGMENTER_H

#include <fstream>
#include <boost/filesystem.hpp>
#include "BaseTypes.h"

namespace liber {
namespace rsync {

class SegmentReceiver;
class SegmentHook;
struct SegmentationReport;

class Segmenter {
public:

  enum SegmentDistance {

    // The requester does not overlap.
    FullStride,

    // The Authority creates a segment for every offset.
    EveryOffset
  };

  explicit Segmenter(SegmentDistance distance = FullStride);

  bool process(std::istream& istream,
               SegmentReceiver& rReceiver,
               ui32 nSegmentSizeBytes,
               SegmentationReport& rReport);

private:

  bool processFullStride(std::istream& istream,
               SegmentReceiver& rReceiver,
               ui32 nSegmentSizeBytes,
               SegmentationReport& rReport);

  bool processEveryOffset(std::istream& istream,
               SegmentReceiver& rReceiver,
               ui32 nSegmentSizeBytes,
               SegmentationReport& rReport);

private:

  SegmentDistance mDistance;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENTER_H
