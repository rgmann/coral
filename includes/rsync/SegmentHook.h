#ifndef RSYNC_SEGMENT_HOOK_H
#define RSYNC_SEGMENT_HOOK_H

#include "SegmentReceiver.h"

namespace liber {
namespace rsync {

// Particularly used by the FileAuthority to look up each of the Segments
// as they are created.
class SegmentHook : public liber::rsync::SegmentReceiver {
public:

  SegmentHook();
  virtual ~SegmentHook();

  void push(Segment* pSegment);

  /**
   * Derived class overrides call and does something with the Segment.
   * The call method should transfer control of the Segment or destroy
   * the segment.
   */
  virtual void call(Segment* pSegment) = 0;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_HOOK_H
