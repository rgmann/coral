#include "SegmentFile.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
SegmentFile::
SegmentFile(const liber::rsync::JobDescriptor& descriptor)
: SegmentAccessor()
, mDescriptor(descriptor)
{
  mIStream.open(mDescriptor.getPath().generic_string(), std::ifstream::binary);
}

//-----------------------------------------------------------------------------
SegmentFile::~SegmentFile()
{
  mIStream.close();
}

//-----------------------------------------------------------------------------
Segment* SegmentFile::getSegment(liber::rsync::Segment::ID id)
{
  Segment* lpSegment = NULL;

  if ((id >= 0) && mIStream.is_open())
  {
    ui8* lpData = new ui8[mDescriptor.getSegmentSize()];

    mIStream.seekg(id * mDescriptor.getSegmentSize(), mIStream.beg);
    mIStream.read((char*)lpData, mDescriptor.getSegmentSize());

    lpSegment = new Segment(id, id * mDescriptor.getSegmentSize());
    lpSegment->setData(lpData, mDescriptor.getSegmentSize(), mIStream.gcount());

    delete[] lpData;
  }

  return lpSegment;
}


