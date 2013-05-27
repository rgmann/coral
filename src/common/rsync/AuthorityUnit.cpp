#include "AuthorityUnit.h"

//------------------------------------------------------------------------------
AuthorityUnit::AuthorityUnit(Queue<RsyncPacket*>& outQueue)
: WorkUnit(outQueue)
, m_pSegmentTable(NULL)
{
}

//------------------------------------------------------------------------------
AuthorityUnit::~AuthorityUnit()
{
   
}

//------------------------------------------------------------------------------
void AuthorityUnit::setRoot(const std::string& root)
{
   m_authority.setRoot(root);
}

//------------------------------------------------------------------------------
bool AuthorityUnit::work()
{
   RsyncSegmentReportPacket*  l_pRptPacket = NULL;
   RsyncAssemblyInstr*        l_pInstr = NULL;
   RsyncPacket*               l_pPacket = NULL;
   
   // Attempt to get a packet from the queue.
   if (!popInputPacket(&l_pPacket)) return false;
   
   // Convert from RsyncPacket to RsyncSegmentReportPacket.
   if (!l_pPacket->to(&l_pRptPacket))
   {
      delete l_pPacket;
      return false;
   }
   
   // If we got an input packet, add it to the table.
   if (l_pRptPacket->type() == RsyncSegmentReportPacket::HeaderType)
   {
      if (m_pSegmentTable)
      {
         // Getting here means that we did not get all the segments for the
         // last file.  Therefore, we need to blow away the table.
         delete m_pSegmentTable;
         m_pSegmentTable = NULL;
      }
      
      m_pSegmentTable = new RsyncSegmentTable();
   }
   else if (m_pSegmentTable == NULL)
   {
      printf("AuthorityUnit::work: NULL segment table on non header segment\n");
      delete l_pRptPacket;
      return false;
   }
   
   // Add the segment report packet to the segment table.
   if (!m_pSegmentTable->addSegment(l_pRptPacket))
   {
      printf("AuthorityUnit::work: Failed adding segment to table\n");
      delete l_pRptPacket;
      return false;
   }
   
   // Check wether the segment report is complete (that is, we've received the
   // end marker).
   if (m_pSegmentTable->isComplete())
   {
      if (!m_authority.process(m_pSegmentTable))
      {
         printf("AuthorityUnit::work: Failed to process segment hash.\n");
         return false;
      }
      
      // Push the assembly instructions to the queue.
      while (m_authority.getNextInstruction(&l_pInstr))
      {
         l_pPacket = new RsyncPacket(l_pInstr);
         if (!pushOutputPacket(l_pPacket))
         {
            printf("AuthorityUnit::work: Failed to push assembly instr\n");
         }
         l_pPacket = NULL;
      }
      
      m_pSegmentTable = NULL;
   }
   
   if (l_pRptPacket)
   {
      delete l_pRptPacket;
      l_pRptPacket = NULL;
   }
   
   return true;
}
