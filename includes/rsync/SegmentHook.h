#ifndef RSYNC_SEGMENT_HOOK_H
#define RSYNC_SEGMENT_HOOK_H

#include "SegmentReceiver.h"

namespace liber {
namespace rsync {

// Particularly used by the FileAuthority to look up each of the Segments
// as they are created.
class SegmentHook : public SegmentReceiver {
public:

  SegmentHook();
  virtual ~SegmentHook();

  void push(Segment& rSegment);

  /**
   * Derived class overrides call and does something with the Segment.
   * The call method should transfer control of the Segment or destroy
   * the segment.
   */
  virtual void call(Segment& rSegment) = 0;

private:

  SegmentHook(const SegmentHook&);
  SegmentHook& operator= (const SegmentHook&);
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_HOOK_H
