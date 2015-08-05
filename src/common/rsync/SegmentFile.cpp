#include "Log.h"
#include "FileSystemInterface.h"
#include "JobDescriptor.h"
#include "SegmentFile.h"

using namespace liber;
using namespace liber::rsync;

//-----------------------------------------------------------------------------
SegmentFile::SegmentFile()
: SegmentAccessor()
, mpDescriptor(NULL)
{
}

//-----------------------------------------------------------------------------
SegmentFile::~SegmentFile()
{
  close();
}

//-----------------------------------------------------------------------------
bool SegmentFile::open( FileSystemInterface& file_sys_interface, const liber::rsync::JobDescriptor& descriptor)
{
  close();
  mpDescriptor = &descriptor;
  return file_sys_interface.open(descriptor.getDestination().path, mIStream);
}

//-----------------------------------------------------------------------------
void SegmentFile::close()
{
  mIStream.close();
}

//-----------------------------------------------------------------------------
Segment* SegmentFile::getSegment(liber::rsync::Segment::ID id)
{
  Segment* lpSegment = NULL;

  if (mpDescriptor)
  {
    if ((id >= 0) && mIStream.is_open())
    {
      mIStream.seekg(id * mpDescriptor->getSegmentSize(), mIStream.beg);
      lpSegment = new Segment(id, id * mpDescriptor->getSegmentSize());
      lpSegment->setData(mIStream, mpDescriptor->getSegmentSize());
    }
  }

  return lpSegment;
}


