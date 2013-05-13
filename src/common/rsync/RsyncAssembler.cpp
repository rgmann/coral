#include "RsyncAssembler.h"
#include <iostream>

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
RsyncAssembler::AddStatus RsyncAssembler::addInstruction(void* pInstr,
                                 unsigned int nInstrLenBytes)
{
   RsyncAssemblyInstr* l_pInstr = NULL;
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
   
   l_pInstr = new RsyncAssemblyInstr();
   if (l_pInstr == NULL)
   {
      std::cout << "addInstruction: Failed to create Instr" << std::endl;
      return Failure;
   }
   
   if (!l_pInstr->unpack(pInstr, nInstrLenBytes))
   {
      std::cout << "addInstruction: Failed to unpack" << std::endl;
      delete l_pInstr;
      l_pInstr = NULL;
      return Failure;
   }
   
   // TODO: Figure out automatic handling of 'Begin' and 'End' instructions.
   
   if (l_pInstr->type() == RsyncAssemblyInstr::RsyncBeginMarker)
   {
      RsyncSegmentReport* l_pSegReport = NULL;
      std::string l_sFilename;
      
      l_sFilename = std::string((char*)l_pInstr->data());

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
   else if (l_pInstr->type() == RsyncAssemblyInstr::RsyncEndMarker)
   {
      std::cout << "Got EndMarker" << std::endl;
      return EndMarker;
   }
   else if (m_eAssemblyMode == StageInstructions)
   {
      m_vAssmbInstrs.push_back(l_pInstr);
   }
   else
   {
      printf("RsyncAssembler::add: exec instr\n");
      if (!execAssemblyInstr(l_pInstr))
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
   
   if (pInstr->type() == RsyncAssemblyInstr::RsyncChunkType)
   {
      l_bExecSuccess = m_pStage->add((unsigned char*)pInstr->data(),
                                     pInstr->dataSize());
   }
   else if (pInstr->type() == RsyncAssemblyInstr::RsyncSegmentType)
   {
      RsyncSegId     l_nSegId = 0;
      unsigned char* l_pData = NULL;
      unsigned int   l_nDataSize = 0;
      
      l_nSegId = pInstr->info();
      if (l_nSegId >= m_vSegments.size())
      {
         return false;
      }
      
//      if (!m_vSegments[l_nSegId]->getData(&l_pData, l_nDataSize))
      if (!m_vSegments[l_nSegId]->getData(m_pStage->stream(),
                                           &l_pData, l_nDataSize))
      {
         return false;
      }
      
      l_bExecSuccess = m_pStage->add(l_pData,
                                     l_nDataSize);
      
      // Local allocated data can now be delete.
      delete[] l_pData;
      l_pData = NULL;
   }
   else
   {
      l_bExecSuccess = false;
   }
   
   return l_bExecSuccess;
}
