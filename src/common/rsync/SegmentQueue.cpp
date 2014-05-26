#include "Log.h"
#include "SegmentQueue.h"

using namespace liber;
using namespace liber::rsync;

//----------------------------------------------------------------------------
SegmentQueue::SegmentQueue()
: SegmentReceiver()
{
  mSegments.initialize();
}

//----------------------------------------------------------------------------
SegmentQueue::~SegmentQueue()
{
}

//----------------------------------------------------------------------------
void SegmentQueue::push(Segment& rSegment)
{
  mSegments.push(new Segment(rSegment));
}

//----------------------------------------------------------------------------
void SegmentQueue::push(Segment* pSegment)
{
  mSegments.push(pSegment);
}

//----------------------------------------------------------------------------
bool SegmentQueue::pop(Segment** ppSegment, int nTimeoutMs)
{
  return mSegments.pop(*ppSegment, nTimeoutMs);
}

