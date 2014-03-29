#ifndef RSYNC_SEGMENT_HASH_H
#define RSYNC_SEGMENT_HASH_H

#include "Segment.h"
#include "SimpleHashTable.h"

namespace liber {
namespace rsync {

class SegmentHash {
public:

  SegmentHash();
  ~SegmentHash();

  /**
   * Insert a Segment.  Returns false on MD5 collision.
   */
  bool insert(Segment* pSegment);

  Segment* find(Segment* pSegment, bool bRemoveIfFound = false);

  /**
   * Queries the SegmentHash for the Segment.
   *
   * @return  True if an exact match is found; false otherwise.
   */
  bool include(Segment* pSegment, bool bRemoveIfFound = false);

private:

  SimpleHashTable<16, Segment*> mHash;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_HASH_H
