#ifndef RSYNC_SEGMENTER_H
#define RSYNC_SEGMENTER_H

#include <fstream>
#include <boost/filesystem.hpp>
#include "BaseTypes.h"

namespace liber {
namespace rsync {

class SegmentReceiver;
class SegmentHook;
class SegmentationReport;

namespace Segmenter {
  
  bool processFullStride(
    std::istream&       istream,
    SegmentReceiver&    segment_receiver,
    ui32                segment_size_bytes,
    SegmentationReport& report
  );

  bool processEveryOffset(
    std::istream&       istream,
    SegmentReceiver&    segment_receiver,
    ui32                segment_size_bytes,
    SegmentationReport& report);

} // End namespace Segmenter

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENTER_H
