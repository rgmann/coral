#ifndef RSYNC_SEGMENT_QUEUE_H
#define RSYNC_SEGMENT_QUEUE_H

#include "Queue.h"
#include "SegmentReceiver.h"

namespace liber {
namespace rsync {

class SegmentQueue : public SegmentReceiver {
public:

  SegmentQueue();
  ~SegmentQueue();

  void push(Segment& rSegment);
  void push(Segment* pSegment);

  bool pop(Segment** ppSegment, int nTimeoutMs);

private:

  SegmentQueue(const SegmentQueue&);
  SegmentQueue& operator= (const SegmentQueue&);

private:

  Queue<Segment*> mSegments;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_SEGMENT_QUEUE_H
