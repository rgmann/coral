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

   void push( Segment& segment );
   void push( Segment* segment_ptr );

   bool pop( Segment** ppSegment, int timeout_ms );

private:

   SegmentQueue(const SegmentQueue&);
   SegmentQueue& operator= (const SegmentQueue&);

private:

   Queue<Segment*> segments_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_SEGMENT_QUEUE_H
