#include "ChatLog.h"

//------------------------------------------------------------------------------
ChatLog::ChatLog()
{
   m_nCurrentTs = 0;
}

//------------------------------------------------------------------------------
ui32 ChatLog::append(ChatMsg &msg)
{
   m_nCurrentTs++;
   
   msg.setTs(m_nCurrentTs);
   m_vMsgs.push_back(msg);
   
   return m_nCurrentTs;
}

//------------------------------------------------------------------------------
ui32 ChatLog::read(ui32 callerId, ui32 sinceTs, std::vector<ChatMsg*> &msgVec)
{
   ui32  l_nInd = 0;
   ui32  l_nLastTs = 0;
   
   if (m_vMsgs.empty()) return 0;
   
   if (sinceTs >= m_vMsgs.size())
   {
      return m_vMsgs[m_vMsgs.size() - 1].getTs();
   }
   
   // Take extremely naive approach and assume ts = index
   for (l_nInd = sinceTs; l_nInd < m_vMsgs.size(); ++l_nInd)
   {
      msgVec.push_back(&m_vMsgs[l_nInd]);
   }
   
   if (!msgVec.empty())
   {
      l_nLastTs = m_vMsgs[m_vMsgs.size() - 1].getTs();
   }
   
   return l_nLastTs;
}
