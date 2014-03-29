#include "SegmentHook.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
SegmentHook::SegmentHook()
{
}

//-----------------------------------------------------------------------------
SegmentHook::~SegmentHook()
{
}

//-----------------------------------------------------------------------------
void SegmentHook::push(Segment* pSegment)
{
  call(pSegment);
}

