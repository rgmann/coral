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

std::ofstream authLog;

//-----------------------------------------------------------------------------
Authority::
Authority(JobDescriptor& rDescriptor,
          HashTable<16, Segment*>&   rHash, 
          InstructionReceiver& rReceiver)
: SegmentHook()
, mrDescriptor(rDescriptor)
, mrHash(rHash)
, mrReceiver(rReceiver)
, mSegmenter(Segmenter::EveryOffset)
, mnSegmentSkipCount(0)
, mnMaxChunkSize(512)
, mnBufferedCount(0)
, mBufferStartID(-1)
, mnTotalSegmentBytes(0)
, mnSegmentBytes(0)
, mnChunkBytes(0)
{
  mChunkBuffer.allocate(mnMaxChunkSize);
  authLog.open("/Users/vaughanbiker/Development/liber/test/rsync/test_root/auth_log.csv");
}

//-----------------------------------------------------------------------------
Authority::~Authority()
{
  authLog.close();
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
}

//-----------------------------------------------------------------------------
bool Authority::process(std::istream& istream, JobReport::SourceReport& rReport)
{
  reset();

  mpReport = &rReport.authority;
  mpReport->receivedSegmentCount = mrHash.size();
  mpReport->authBegin.sample();

  mrReceiver.push(new BeginInstruction(mrDescriptor));
  bool lbSuccess = mSegmenter.process(istream, *this,
                                      mrDescriptor.getSegmentSize(),
                                      rReport.segmentation);
  mpReport->authEnd.sample();
  mrReceiver.push(new EndInstruction());

  mpReport = NULL;

  return lbSuccess;
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
    mrReceiver.push(lpInstruction);

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
//    if (mrHash.find(pSegment->getWeak().checksum(), lpMatchSegment, comparator))
    if (mrHash.remove(rSegment.getWeak().checksum(), lpMatchSegment, comparator))
    {
      // Any data in the buffer when a match occurs should be moved to a chunk
      // instruction and sent before sending the ID of the matched segment.
      flushChunkBuffer((mBufferStartID == -1) ? FlushAll : (rSegment.getID() - mBufferStartID));

//      log::debug("Match found: %d\n", pSegment->getID());
      // If the Segment exists in the hash, create a Segment instruction.
      if (lpMatchSegment)
      {
        mrReceiver.push(new SegmentInstruction(lpMatchSegment->getID()));
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
//      log::status("Authority: match at %d\n", rSegment.getID());
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
//          log::status("Authority: mChunkBuffer.size=%u\n", mChunkBuffer.size());
          if (mChunkBuffer.isEmpty()) mBufferStartID = rSegment.getID();
          ui32 lnBytesWritten = mChunkBuffer.write((const char*)rSegment.data(), rSegment.size());
          mnBufferedCount = rSegment.size() - 1;
          
//          log::status("Authority: push back exp=%d bytes, act = %u, size=%u\n", rSegment.size(), lnBytesWritten, mChunkBuffer.size());
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



