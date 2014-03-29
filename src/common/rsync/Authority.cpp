#include <iostream>
#include "Authority.h"

#undef DEBUG_RSYNC_AUTHORITY

using namespace liber::rsync;

//-----------------------------------------------------------------------------
Authority::
Authority(JobDescriptor& rDescriptor,
          SegmentHash&   rHash, 
          InstructionReceiver& rReceiver)
: mDescriptor(rDescriptor)
, mrHash(rHash)
, mrReceiver(rReceiver)
, mnMaxChunkSize(512)
{
}

//-----------------------------------------------------------------------------
Authority::~Authority()
{
}

//-----------------------------------------------------------------------------
bool Authority::process(std::istream& istream)
{
  Segmenter lSegmenter(mDescriptor.getSegmentSize(), Segmenter::EveryOffset);

  mrReceiver.push(new BeginInstruction(mDescriptor));
  bool lbSuccess = lSegmenter.process(istream, *this);
  mrReceiver.push(new EndInstruction());

  return lbSuccess;
}

//-----------------------------------------------------------------------------
Instruction* Authority::createInstruction(std::stringstream& stream)
{
  stream.seekg (0, stream.end);
  int lnLength = stream.tellg();
  stream.seekg (0, stream.beg);

  ChunkInstruction* lpInstruction = new ChunkInstruction(lnLength);
  if (lpInstruction->data())
  {
    stream.read((char*)lpInstruction->data(), lpInstruction->size());
#ifdef DEBUG_RSYNC_AUTHORITY
    std::cout << "read " << stream.gcount() << std::endl;
#endif
    stream.clear();
    stream.str("");
  }

  return lpInstruction;
}

//-----------------------------------------------------------------------------
void Authority::call(Segment* pSegment)
{
#ifdef DEBUG_RSYNC_AUTHORITY
  std::cout << "Authority::call( "
            << std::hex << pSegment->getWeak().checksum()
            << ", " << pSegment->size() << std::dec << " ): ";
#endif

  if (mnSegmentSkipCount == 0)
  {
    Segment* lpMatchSegment = mrHash.find(pSegment);
    if (lpMatchSegment)
    {
      // Any data in the buffer when a match occurs should be moved to a chunk
      // instruction and sent before sending the ID of the matched segment.
      if (mChunkBuffer.str().size() > 0)
      {
        mrReceiver.push(createInstruction(mChunkBuffer));
      }

#ifdef DEBUG_RSYNC_AUTHORITY
      std::cout << "Match found: " << pSegment->getID() << std::endl;
#endif
      // If the Segment exists in the hash, create a Segment instruction.
      mrReceiver.push(new SegmentInstruction(lpMatchSegment->getID()));

      // Since the Authority creates a segment for every byte offset, a match
      // allows the Authority to skip the next N-1 segments for a Segment size
      // of N bytes.
      mnSegmentSkipCount = pSegment->size() - 1;
    }
    else
    {
#ifdef DEBUG_RSYNC_AUTHORITY
      std::cout << "No match. Adding 0x" << std::hex << (ui16)pSegment->data()[0] << std::endl;
#endif

      // If the segment is not found in the hash, push the first byte
      mChunkBuffer.put(pSegment->data()[0]);

      if (mChunkBuffer.str().size() > mnMaxChunkSize)
      {
        mrReceiver.push(createInstruction(mChunkBuffer));
      }
    }
  }
  else
  {
#ifdef DEBUG_RSYNC_AUTHORITY
    std::cout << "Skipping" << std::endl;
#endif
    mnSegmentSkipCount--;
  }

  // The Segment is no longer needed.
  delete pSegment;
}



