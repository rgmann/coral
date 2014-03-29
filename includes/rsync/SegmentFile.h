#ifndef RSYNC_SEGMENT_FILE_H
#define RSYNC_SEGMENT_FILE_H

#include <fstream>
#include "Segmenter.h"
#include "SegmentAccessor.h"

namespace liber {
namespace rsync {

// Since SegmentIDs are simply sequential indeces of blocks, it is simple
// enough to open the file and seek forward into the file if the chunk size
// is known.
class SegmentFile : public liber::rsync::SegmentAccessor {
public:

  SegmentFile(const liber::rsync::JobDescriptor&);
  ~SegmentFile();

  Segment* getSegment(liber::rsync::Segment::ID id);

private:

  liber::rsync::JobDescriptor mDescriptor;
  std::ifstream mIStream;

};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_FILE_H
