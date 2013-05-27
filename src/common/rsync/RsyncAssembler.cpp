#include <iostream>
#include "RsyncAssembler.h"
#include "AssemblyBeginMarker.h"
#include "AssemblyEndMarker.h"
#include "AssemblyChunkPacket.h"
#include "AssemblySegmentPacket.h"

//------------------------------------------------------------------------------
RsyncAssembler::RsyncAssembler()
{
}

//------------------------------------------------------------------------------
RsyncAssembler::~RsyncAssembler()
{
}

//------------------------------------------------------------------------------
void RsyncAssembler::setSegmenter(RsyncSegmenter* pSegmenter)
{
   m_pSegmenter = pSegmenter;
}

//------------------------------------------------------------------------------
void RsyncAssembler::start(RsyncAssemblyMode mode)
{
   m_eAssemblyMode = mode;
}
//bool RsyncAssembler::start(RsyncAssemblySeqMarker *pMarker,
//                           RsyncAssemblyMode mode)
//{
//   RsyncSegmentReport* l_pSegReport = NULL;
//   std::string l_sFilename;
//   
//   m_eAssemblyMode = mode;
//   
//   if (pMarker == NULL)
//   {
//      return false;
//   }
//   
//   m_pAssemblyMarker = pMarker;
//   
//   // Ask the segmenter for the segment vector associate with the file.
//   l_sFilename = std::string(pMarker->fullFileName);
//   l_pSegReport = m_pSegmenter->getReport(l_sFilename);
//   m_vSegments = l_pSegReport->segments();
//   
//   // Open the stage file.
//   m_pStage = new RsyncFileStage();
//   if (m_pStage == NULL)
//   {
//      return false;
//   }
//   
//   return m_pStage->open(l_sFilename);
//}


//------------------------------------------------------------------------------
//RsyncAssembler::AddStatus RsyncAssembler::addInstruction(void* pInstr,
//                                 unsigned int nInstrLenBytes)
RsyncAssembler::AddStatus
RsyncAssembler::addInstruction(RsyncAssemblyInstr* pInstr)
{
   //RsyncAssemblyInstr* l_pInstr = NULL;
   bool l_bSuccess = false;
   
   if (pInstr == NULL)
   {
      std::cout << "addInstruction: Null instruction" << std::endl;
      return Failure;
   }
   
   if (m_pSegmenter == NULL)
   {
      std::cout << "addInstruction: segmenter not set" << std::endl;
      return Failure;
   }
   
//   l_pInstr = new RsyncAssemblyInstr();
//   if (l_pInstr == NULL)
//   {
//      std::cout << "addInstruction: Failed to create Instr" << std::endl;
//      return Failure;
//   }
//   
//   if (!l_pInstr->unpack(pInstr, nInstrLenBytes))
//   {
//      std::cout << "addInstruction: Failed to unpack" << std::endl;
//      delete l_pInstr;
//      l_pInstr = NULL;
//      return Failure;
//   }
   
   
   if (pInstr->type() == RsyncAssemblyInstr::BeginMarkerType)
   {
      RsyncSegmentReport* l_pSegReport = NULL;
      AssemblyBeginMarker* pBeginMarker = NULL;
      std::string l_sFilename;
      
      //pBeginMarker = static_cast<AssemblyBeginMarker*>(pInstr);
      if (!pInstr->to(&pBeginMarker)) printf("Failed ->to(BeginMarkerType)\n");
      pBeginMarker->getPath(l_sFilename);

      l_pSegReport = m_pSegmenter->getReport(l_sFilename);
      m_vSegments = l_pSegReport->segments();
      
      // Open the stage file.
      m_pStage = new RsyncFileStage();
      if (m_pStage == NULL)
      {
         return Failure;
      }
      
      l_bSuccess = m_pStage->open(l_pSegReport->fullPath());
      if (!l_bSuccess)
      {
         std::cout << "RsyncAssembler::add: Failed to open stage file for "
         << l_pSegReport->fullPath() << std::endl;
         return Failure;
      }
      
      std::cout << "Got BeginMarker" << std::endl;
      
      return BeginMarker;
   }
   else if (pInstr->type() == RsyncAssemblyInstr::EndMarkerType)
   {
      std::cout << "Got EndMarker" << std::endl;
      return EndMarker;
   }
   else if (m_eAssemblyMode == StageInstructions)
   {
      m_vAssmbInstrs.push_back(pInstr);
   }
   else
   {
      printf("RsyncAssembler::add: exec instr\n");
      if (!execAssemblyInstr(pInstr))
      {
         printf("RsyncAssembler::add: Failed to execute instr\n");
      }
   }
   
   return AddOk;
}

//------------------------------------------------------------------------------
bool RsyncAssembler::assemble()
{
   bool l_bSuccess = true;
   
   // If the instructions were staged
   if (m_eAssemblyMode == StageInstructions)
   {
      std::vector<RsyncAssemblyInstr*>::iterator l_iInstrIt;
      
      l_iInstrIt = m_vAssmbInstrs.begin();
      for (; l_iInstrIt < m_vAssmbInstrs.end(); ++l_iInstrIt)
      {
         l_bSuccess = execAssemblyInstr(*l_iInstrIt);
         
         if (!l_bSuccess)
         {
            break;
         }
      }
   }
   
   // Close the stage file now regardless of whether execution was successful.
   m_pStage->close();
   
   if (l_bSuccess)
   {
      // Finally, swap the files.
      l_bSuccess = m_pStage->swap();
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool RsyncAssembler::execAssemblyInstr(RsyncAssemblyInstr* pInstr)
{
   bool l_bExecSuccess = true;
   
   if (pInstr->type() == RsyncAssemblyInstr::ChunkType)
   {
      AssemblyChunkPacket* l_pChunkPkt = NULL;
      
      pInstr->to(&l_pChunkPkt);
      
      l_bExecSuccess = m_pStage->add(l_pChunkPkt->data(),
                                     l_pChunkPkt->dataSize());
   }
   else if (pInstr->type() == RsyncAssemblyInstr::SegmentType)
   {
      AssemblySegmentPacket*  l_pSegPkt = NULL;
      RsyncSegment*           l_pSegment = NULL;
      
      ui32           l_nSegId = 0;
      unsigned char* l_pData = NULL;
      ui32           l_nDataSize = 0;
      
//      l_pSegPkt = dynamic_cast<AssemblySegmentPacket*>(pInstr);
      pInstr->to(&l_pSegPkt);
      l_pSegPkt->getSegmentId(l_nSegId);
      
      if (l_nSegId >= m_vSegments.size())
      {
         return false;
      }
      
      // Get the segment structure with the specified segment ID.  The segment
      // IDs are simply consecutive indices.
      l_pSegment = m_vSegments[l_nSegId];
      
      // A segment indicates that the data is available from the local file.
      // Therefore, read the data associated with the segment directly from
      // the local file and send to the local data buffer (which is allocated
      // by getData()).
      if (!l_pSegment->getData(m_pStage->stream(), &l_pData, l_nDataSize))
      {
         return false;
      }
      
      // Add the new data chunk to the file stage.
      l_bExecSuccess = m_pStage->add(l_pData, l_nDataSize);
      
      // Local allocated data can now be delete.
      delete[] l_pData;
      l_pData = NULL;
      
      delete l_pSegPkt;
      l_pSegPkt = NULL;
   }
   
   // Unrecognized packet type.
   else
   {
      l_bExecSuccess = false;
   }
   
   return l_bExecSuccess;
}
