#include "RsyncJob.h"

using namespace liber::rsync;

//-----------------------------------------------------------------------------
RsyncJob::RsyncJob(RsyncJobType type, const FilePath& source, const FilePath& destination)
: mType(type)
, mSourcePath(source)
, mDestPath(destination)
, mpSegmenter(NULL)
, mpAssembler(NULL)
, mpAuthority(NULL)
{
  if (type == RsyncPull)
  {
    mpSegmenter = new RsyncSegmenter();
    mpAssembler = new RsyncAssembler();
    mpAssembler->setSegmenter(mpAssembler);
  }
  else if (type == RyncPush)
  {
    mpAuthority = new RsyncFileAuthority();
  }
}

//-----------------------------------------------------------------------------
RsyncJob::~RsyncJob()
{
  if (mpAssembler)
  {
    delete mpAssembler;
    mpAssembler = NULL;
  }

  if (mpSegmenter)
  {
    delete mpSegmenter;
    mpSegmenter = NULL;
  }

  if (mpAuthority)
  {
    delete mpAuthority;
    mpAuthority = NULL;
  }
}

//-----------------------------------------------------------------------------
RsyncPacket* RsyncJob::getFileQuery() const
{
  RsyncPacket* lpRsyncPacket = new RsyncPacket(RsyncFileQueryType, lpFileQuery->allocatedSize());
  return lpRsyncPacket;
}

//-----------------------------------------------------------------------------
bool RsyncJob::sendSegments(Queue<RsyncPacket*>& rOutQueue)
{
  // Create a segment report for the file and send the segments.
  if (!mpSegmenter->process(mSource.getFullPath()))
  {
    printf("SegmenetUnit failed to segment %s\n", mSource.getFullPath().c_str());
    return false;
  }

  // Push the header into the output queue.
  // Once processing is complete, we can retrieve the segmenation report.
  RsyncSegmentReport* lpSegmentReport = mSegmenter.getReport(mSource.getFullPath());
  if (lpSegmentReport == NULL)
  {
    printf("Failed to get Segmentation Report\n");
    return false;
  }

  if (!lpSegmentReport->header(&lpReportHdr))
  {
    printf("Failed to get Segmentation Report header\n");
    return false;
  }

  // Push the report header and packed segments to the hash table on the
  // server.
  lpSegRepPkt = new RsyncSegmentReportPacket(l_pReportHdr);
  lpRsyncPacket = new RsyncPacket(l_pSegRepPkt);
  rOutQueue.push(lpRsyncPacket, 100);

  // The header packet is not longer needed, so delete it.
  delete lpSegRepPkt;
  lpSegRepPkt = NULL;

  // Push all segments into the output queue.
  while (lpReport->nextSegment(&lpSegment))
  {
    lpSegRepPkt = new RsyncSegmentReportPacket(lpSegment);
    lpRsyncPacket = new RsyncPacket(RsyncSegmentType, lpSegRepPkt->allocatedSize());

    if (!rOutQueue.push(lpRsyncPacket, 100))
    {
      printf("Failed to add seg #%d\n", lnSegCount);
    }

    // The header packet is not longer needed, so delete it.
    delete lpSegRepPkt;
    lpSegRepPkt = NULL;
    lnSegCount++;

    // Segment object is maintained by the RsyncSegmentReport,
    // so don't delete it.
    lpSegment = NULL;
  }

  return true;
}

//-----------------------------------------------------------------------------
bool RsyncJob::reassemble(RsyncJob* pJob)
{
  bool lbSuccess = false;
  RsyncAssemblyInstr*        lpInstr = NULL;
  RsyncPacket*               lpPacket = NULL;
  RsyncAssembler::AddStatus  lStatus = RsyncAssembler::AddOk;

  // Receive packets until we timeout prematurely or we receive the reassembly
  // end marker packet.
  while (mInQueue.pop(&lpRsyncPacket))
  {
    if (!lpInstr->unpack(lpRsyncPacket->dataPtr(), lpRsyncPacket->allocatedSize()))
    {
      printf("AssemblerUnit::work: Failed to get assembly instr from "
             "RsyncPacket\n");
      return false;
    }

    lStatus = mpAssembler->addInstruction(lpInstr);
    if (lStatus == RsyncAssembler::Failure)
    {
      printf("AssemblerUnit::work: Failed to add instruction\n");
    }
    else if (lStatus == RsyncAssembler::EndMarker)
    {
      // Since we specified NoStaging, the instructions were automatically
      // executed as they were added.  Therefore, calling "assemble" simply
      // replaces the real file with the stage file.

      lbSuccess = mpAssembler->assemble();
      if (!lbSuccess)
      {
         printf("AssemblerUnit::work: Failed to assemble file.\n");
      }
    }

    if (lpInstr)
    {
      delete lpRsyncPacket;
      lpRsyncPacket = NULL;
    }
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool RsyncJob::receiveSegments(RsyncJob* pJob)
{
  RsyncSegmentReportPacket*  lpRptPacket = NULL;
  RsyncAssemblyInstr*        lpInstr = NULL;
  RsyncPacket*               lpRsyncPacket = NULL;
  RsyncSegmentTable*         lpSegmentTable = new RsynSegmentTable();

  if (mInQueue.pop(lpRsyncPacket, 1000))
  {
    lpRptPacket = new RsyncSegmentReportPacket();
    lpRptPacket->unpack(lpRsyncPacket->dataPtr(), lpRsyncPacket->data()->length);

    // If we got an input packet, add it to the table.
    if (lpRptPacket->type() == RsyncSegmentReportPacket::HeaderType)
    {
      // Don't do anything with the header.
    }
    else
    {
      // Add the segment report packet to the segment table.
      if (!lpSegmentTable->addSegment(lpRptPacket))
      {
        printf("AuthorityUnit::work: Failed adding segment to table\n");
        delete lpRptPacket;
        return false;
      }
    }
  }
  else
  {
    return false;
  }

  // Check wether the segment report is complete (that is, we've received the
  // end marker).
  if (lpSegmentTable->isComplete())
  {
    // TODO for server: Check if file exists.  If it does not, create an 
    // empty local file and send an empty segment report to the client.  The
    // client behaves as the authority for the new file and sends back an
    // assembly instruction list entirely composed of chunks.  The empty
    // local file is required because the Assembler expects the file to exist.
    if (!mpAuthority->process(lpSegmentTable))
    {
      printf("AuthorityUnit::work: Failed to process segment hash.\n");
      return false;
    }

    // Push the assembly instructions to the queue
    while (mpAuthority->getNextInstruction(&lpInstr))
    {
      lpRsyncPacket = new RsyncPacket(RsyncAssemblyInstrType, lpInstr->allocatedSize());
      memcpy(lpRsyncPacket->dataPtr(), lpInstr->basePtr(), lpInstr->allocatedSize());

      if (!rOutQueue.push(lpRsyncPacket, 100))
      {
        printf("AuthorityUnit::work: Failed to push assembly instr\n");
      }
      lpRsyncPacket = NULL;
    }
  }

  return lbSuccess;
}


