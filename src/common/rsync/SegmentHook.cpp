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
void SegmentHook::push(Segment& rSegment)
{
  call(rSegment);
}

