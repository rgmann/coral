#include "AssemblerUnit.h"

//------------------------------------------------------------------------------
AssemblerUnit::AssemblerUnit(Queue<RsyncPacket*>& outQueue,
                             RsyncSegmenter* pSegmenter)
: WorkUnit(outQueue)
{
   setSegmenter(pSegmenter);
   
   //
   m_assembler.start(RsyncAssembler::NoStaging);
}

//------------------------------------------------------------------------------
AssemblerUnit::~AssemblerUnit()
{
   // Nothing to deallocate.
}

//------------------------------------------------------------------------------
void AssemblerUnit::setSegmenter(RsyncSegmenter* pSegmenter)
{
   if (pSegmenter)
   {
//      m_pSegmenter = pSegmenter;
      m_assembler.setSegmenter(pSegmenter);
   }
}

//------------------------------------------------------------------------------
bool AssemblerUnit::work()
{
   bool l_bSuccess = false;
   RsyncAssemblyInstr*        l_pInstr = NULL;
   RsyncPacket*               l_pPacket = NULL;
   RsyncAssembler::AddStatus  l_status = RsyncAssembler::AddOk;
   
   if (!popInputPacket(&l_pPacket)) return false;
   
   if (!l_pPacket->to(&l_pInstr))
   {
      printf("AssemblerUnit::work: Failed to get assembly instr from "
             "RsyncPacket\n");
      return false;
   }
   
   l_status = m_assembler.addInstruction(l_pInstr);
   if (l_status == RsyncAssembler::Failure)
   {
      printf("AssemblerUnit::work: Failed to add instruction\n");
   }
   else if (l_status == RsyncAssembler::EndMarker)
   {
      // Since we specified NoStaging, the instructions were automatically
      // executed as they were added.  Therefore, calling "assemble" simply
      // replaces the real file with the stage file.
      
      l_bSuccess = m_assembler.assemble();
      if (!l_bSuccess)
      {
         printf("AssemblerUnit::work: Failed to assemble file.\n");
      }
   }
   
   if (l_pInstr)
   {
      delete l_pPacket;
      l_pPacket = NULL;
   }
   
   return l_bSuccess;
}
