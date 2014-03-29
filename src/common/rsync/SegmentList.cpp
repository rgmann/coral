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
void SegmentList::push(Segment* pSegment)
{
  if (pSegment)
  {
    mSegments.push_back(pSegment);
  }
}

//-----------------------------------------------------------------------------
std::vector<Segment*>& SegmentList::segments()
{
  return mSegments;
}

