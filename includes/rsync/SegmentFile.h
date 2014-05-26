#ifndef RSYNC_SEGMENT_FILE_H
#define RSYNC_SEGMENT_FILE_H

#include <fstream>
#include "Segmenter.h"
#include "SegmentAccessor.h"

namespace liber {
namespace rsync {

class JobDescriptor;
class FileSystemInterface;

// Since SegmentIDs are simply sequential indeces of blocks, it is simple
// enough to open the file and seek forward into the file if the chunk size
// is known.
class SegmentFile : public SegmentAccessor {
public:

  SegmentFile(FileSystemInterface&);
  ~SegmentFile();

  bool open(const JobDescriptor& descriptor);
  void close();

  Segment* getSegment(Segment::ID id);

private:

  const JobDescriptor* mpDescriptor;
  std::ifstream mIStream;

  FileSystemInterface& mrFileSys;
};

} // End namespace rsync
} // End namespace liber


#endif // RSYNC_SEGMENT_FILE_H
