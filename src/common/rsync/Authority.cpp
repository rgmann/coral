#include <iostream>
#include <fstream>
#include <algorithm>
#include "Log.h"
#include "Segmenter.h"
#include "JobDescriptor.h"
#include "InstructionReceiver.h"
#include "Authority.h"

using namespace liber;
using namespace liber::rsync;


//-----------------------------------------------------------------------------
Authority::Authority()
: SegmentHook()
, mSegmenter(Segmenter::EveryOffset)
, mnSegmentSkipCount(0)
, mnMaxChunkSize(512)
, mnBufferedCount(0)
, mBufferStartID(-1)
, mnTotalSegmentBytes(0)
, mnSegmentBytes(0)
, mnChunkBytes(0)
, mpReceiver(NULL)
{
  mChunkBuffer.allocate(mnMaxChunkSize);
}

//-----------------------------------------------------------------------------
Authority::~Authority()
{
}

//-----------------------------------------------------------------------------
void Authority::reset()
{
  mnBufferedCount = 0;
  mnSegmentSkipCount = 0;
  mBufferStartID = -1;

  mnTotalSegmentBytes = 0;
  mnSegmentBytes = 0;
  mnChunkBytes = 0;

  mChunkBuffer.clear();

  mpReceiver = NULL;

  SegmentDestructor destructor;
  mHash.clear(destructor);
}

//-----------------------------------------------------------------------------
bool Authority::
process(JobDescriptor& rDescriptor,
        std::istream& istream,
        InstructionReceiver& rReceiver,
        SourceReport& rReport)
{
  mpReport = &rReport.authority;
  mpReport->receivedSegmentCount = mHash.size();
  mpReport->authBegin.sample();

  mpReceiver = &rReceiver;

  mpReceiver->push(new BeginInstruction(rDescriptor));
  bool lbSuccess = mSegmenter.process(istream, *this,
                                      rDescriptor.getSegmentSize(),
                                      rReport.segmentation);
  mpReport->authEnd.sample();
  mpReceiver->push(new EndInstruction());

  mpReport = NULL;
  mpReceiver = NULL;
  
  reset();

  return lbSuccess;
}

//-----------------------------------------------------------------------------
HashTable<16, Segment*>& Authority::hash()
{
  return mHash;
}

//-----------------------------------------------------------------------------
void Authority::flushChunkBuffer(int nFlushCount)
{
  if (!mChunkBuffer.isEmpty())
  {
    // Determine how many bytes to send in the chunk instruction. If the 
    // caller specifies 'FlushAll', all bytes are sent.  Otherwise only the
    // first 'nFlushCount' bytes are sent in the chunk and the remainder are
    // thrown away.
    ui32 lnChunkSize = (nFlushCount == FlushAll) ? mChunkBuffer.size() :
                                                   (ui32)nFlushCount;

    // Allocate the chunk instruction and move the specified number of bytes
    // from the chunk buffer to the chunk instruction.
    ChunkInstruction* lpInstruction = new ChunkInstruction(lnChunkSize);
    if (lpInstruction->data())
    {
      if (mChunkBuffer.read((char*)lpInstruction->data(), lnChunkSize) != lnChunkSize)
      {
        log::error("Authority: Failed to read from chunk buffer\n");
      }

      // Reset the chunk buffer.
      mChunkBuffer.clear();

      mnChunkBytes += lnChunkSize;
    }

    // Send the chunk instruction to the instruction receiver.
    mpReceiver->push(lpInstruction);

    // All segment data has been removed from the buffer.
    mnBufferedCount = 0;
    mBufferStartID = -1;

    mpReport->chunkCount++;
  }
}

//-----------------------------------------------------------------------------
void Authority::call(Segment& rSegment)
{
  Timestamp time;

  if (rSegment.endOfStream())
  {
    // Make sure all chunk data has been sent.
    flushChunkBuffer();
  }
  else if (mnSegmentSkipCount == 0)
  {
    time.sample();

    Segment* lpMatchSegment = NULL;
    SegmentComparator comparator(&rSegment);

    if (mHash.remove(rSegment.getWeak().checksum(), lpMatchSegment, comparator))
    {
      // Any data in the buffer when a match occurs should be moved to a chunk
      // instruction and sent before sending the ID of the matched segment.
      flushChunkBuffer((mBufferStartID == -1) ?
                       FlushAll : (rSegment.getID() - mBufferStartID));

      // If the Segment exists in the hash, create a Segment instruction.
      if (lpMatchSegment)
      {
        mpReceiver->push(new SegmentInstruction(lpMatchSegment->getID()));
      }
      else
      {
        log::error("Authority: NULL match segment retrieved from hash.\n");
      }

      // Since the Authority creates a segment for every byte offset, a match
      // allows the Authority to skip the next N-1 segments for a Segment size
      // of N bytes.
      mnSegmentSkipCount = rSegment.size() - 1;

      mpReport->matchedSegmentCount++;
      mnSegmentBytes += rSegment.size();
      delete lpMatchSegment;
    }
    else
    {
      if (rSegment.data())
      {
        if ((mBufferStartID != -1) && ((rSegment.getID() - mBufferStartID) >= mnMaxChunkSize))
        {
          flushChunkBuffer();
        }

        if (mnBufferedCount == 0)
        {
          if (mChunkBuffer.isEmpty())
          {
            mBufferStartID = rSegment.getID();
          }

          mChunkBuffer.write((const char*)rSegment.data(), rSegment.size());
          mnBufferedCount = rSegment.size() - 1;
        }
        else
        {
          mnBufferedCount--;
        }
      }
      else
      {
        log::debug("NULL data at segment #%d\n", rSegment.getID());
      }
    }
  }
  else
  {
    mnSegmentSkipCount--;
  }

  mnTotalSegmentBytes += rSegment.size();
}



