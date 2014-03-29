#ifndef SEGMENT_LIST_H
#define SEGMENT_LIST_H

#include <vector>
#include "SegmentReceiver.h"

namespace liber {
namespace rsync {


class SegmentList : public liber::rsync::SegmentReceiver {
public:

  SegmentList();
  ~SegmentList();

  void push(Segment* pSegment);
  Segment* pop();

  std::vector<Segment*>& segments();

private:

  std::vector<Segment*> mSegments;
};


} // End namespace rsync
} // End namespace liber

#endif // SEGMENT_LIST_H
