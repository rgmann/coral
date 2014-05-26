#ifndef SEGMENT_RECEIVER_H
#define SEGMENT_RECEIVER_H

#include "Segment.h"

namespace liber {
namespace rsync {

// Common interface expected by Segmenter. Encapsulates output of segmentation
// process.
class SegmentReceiver {
public:

  SegmentReceiver();
  virtual ~SegmentReceiver();

  /**
   * Transfer control of a Segment instance to the
   * receiver.
   */
  virtual void push(Segment& rSegment) = 0;


private:

  SegmentReceiver(const SegmentReceiver&);
  SegmentReceiver& operator= (const SegmentReceiver&);

};

} // End namespace rsync
} // End namespace liber

#endif // SEGMENT_RECEIVER_H
