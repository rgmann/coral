#include <iostream>
#include <fstream>
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
, mnMaxChunkSize(4*512)
, mnBufferedCount(0)
{
  authLog.open("/Users/vaughanbiker/Development/liber/test/rsync/test_root/auth_log.csv");
}

//-----------------------------------------------------------------------------
Authority::~Authority()
{
  authLog.close();
}

//-----------------------------------------------------------------------------
bool Authority::process(std::istream& istream, JobReport::SourceReport& rReport)
{
  mpReport = &rReport.authority;
  mpReport->receivedSegmentCount = mrHash.size();
  mpReport->authBegin.sample();

  mChunkBuffer.reserve(mnMaxChunkSize);

  mrReceiver.push(new BeginInstruction(mrDescriptor));
  bool lbSuccess = mSegmenter.process(istream, *this,
                                      mrDescriptor.getSegmentSize(),
                                      rReport.segmentation);
  mrReceiver.push(new EndInstruction());

  mpReport->authEnd.sample();
  mpReport = NULL;

  return lbSuccess;
}

//-----------------------------------------------------------------------------
Instruction* Authority::createInstruction(std::string& rBuffer)
{
  ChunkInstruction* lpInstruction = new ChunkInstruction(rBuffer.size());
  if (lpInstruction->data())
  {
    memcpy(lpInstruction->data(), rBuffer.data(), lpInstruction->size());
#ifdef DEBUG_RSYNC_AUTHORITY
    std::cout << "read " << stream.gcount() << std::endl;
#endif
    rBuffer.clear();
  }

  return lpInstruction;
}

//-----------------------------------------------------------------------------
void Authority::flushChunkBuffer()
{
  if (mChunkBuffer.size() > 0)
  {
    mrReceiver.push(createInstruction(mChunkBuffer));
    mpReport->chunkCount++;
  }
}

//-----------------------------------------------------------------------------
void Authority::call(Segment& rSegment)
{
//  log::debug("Authority::call( 0x%08X, %d ):",
//             pSegment->getWeak().checksum(),
//             pSegment->size());
//  log::debug("Authority::call( %d, %d ):",
//             pSegment->getID(),
//             pSegment->size());

  Timestamp begin; begin.sample();

  if (rSegment.endOfStream())
  {
    // Do nothing. This segment marks the end of the stream.
    flushChunkBuffer();
  }
  else if (mnSegmentSkipCount == 0)
  {
    Segment* lpMatchSegment = NULL;
    SegmentComparator comparator(&rSegment);
//    if (mrHash.find(pSegment->getWeak().checksum(), lpMatchSegment, comparator))
    if (mrHash.remove(rSegment.getWeak().checksum(), lpMatchSegment, comparator))
    {
      // Any data in the buffer when a match occurs should be moved to a chunk
      // instruction and sent before sending the ID of the matched segment.
      flushChunkBuffer();

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
      delete lpMatchSegment;
    }
    else
    {
      // If the segment is not found in the hash, push the first byte
      if (rSegment.data())
      {
        if (mnBufferedCount == 0)
        {
          mChunkBuffer.append((const char*)rSegment.data(), rSegment.size());
          mnBufferedCount = rSegment.size();
        }
        else
        {
          mnBufferedCount--;
        }

        if (mChunkBuffer.size() > mnMaxChunkSize)
        {
          mrReceiver.push(createInstruction(mChunkBuffer));
          mpReport->chunkCount++;
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

  //authLog << (Timestamp::Now() - begin).fseconds() << std::endl;
}



