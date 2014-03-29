#include "SegmentHash.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
SegmentHash::SegmentHash()
{
}

//-----------------------------------------------------------------------------
SegmentHash::~SegmentHash()
{
}

//-----------------------------------------------------------------------------
bool SegmentHash::insert(Segment* pSegment)
{
  return mHash.add(pSegment->getWeak().checksum(), pSegment);
}

//-----------------------------------------------------------------------------
Segment* SegmentHash::find(Segment* pSegment, bool bRemoveIfFound)
{
  Segment* lpSegment = NULL;
  std::vector<Segment*> lvSegments;

  // First level lookup:
  // Find all Adler32 checksums that have the same 16-bit hash.
  mHash.get(pSegment->getWeak().checksum(), lvSegments);

  // If the list of matches is empty, the segment is not in the table.
  if (lvSegments.empty() == false)
  {
    // Second level lookup:
    // 
    std::vector<Segment*>::iterator liSegment = lvSegments.begin();
    for (; liSegment != lvSegments.end(); ++liSegment)
    {
      lpSegment = *liSegment;
      if (lpSegment->getWeak().checksum() == pSegment->getWeak().checksum())
      {
        // Third level:
        // Check the MD5 checksum.
        if (lpSegment->getStrong() == pSegment->getStrong())
        {
          break;
        }
        else
        {
          lpSegment = NULL;
        }
      }
      else
      {
        lpSegment = NULL;
      }
    }

    if (lpSegment && bRemoveIfFound)
    {
      // TODO: Remove segment from hash table.
    }
  }

  return lpSegment;
}

//-----------------------------------------------------------------------------
bool SegmentHash::include(Segment* pSegment, bool bRemoveIfFound)
{
  return (find(pSegment, bRemoveIfFound) != NULL);
}

