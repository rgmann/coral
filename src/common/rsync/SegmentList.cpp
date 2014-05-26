#include "SegmentList.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
SegmentList::SegmentList()
: SegmentReceiver()
{
}

//-----------------------------------------------------------------------------
SegmentList::~SegmentList()
{
}

//-----------------------------------------------------------------------------
void SegmentList::push(Segment& rSegment)
{
  mSegments.push_back(new Segment(rSegment));
}

//-----------------------------------------------------------------------------
std::vector<Segment*>& SegmentList::segments()
{
  return mSegments;
}

