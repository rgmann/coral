#include "Log.h"
#include "SegmentQueue.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
SegmentQueue::SegmentQueue()
:  SegmentReceiver()
{
}

//----------------------------------------------------------------------------
SegmentQueue::~SegmentQueue()
{
}

//----------------------------------------------------------------------------
void SegmentQueue::push( Segment& segment )
{
   segments_.push( new Segment( segment ) );
}

//----------------------------------------------------------------------------
void SegmentQueue::push( Segment* segment_ptr )
{
   segments_.push( segment_ptr );
}

//----------------------------------------------------------------------------
bool SegmentQueue::pop( Segment** ppSegment, int timeout_ms )
{
   return segments_.pop( *ppSegment, timeout_ms );
}

