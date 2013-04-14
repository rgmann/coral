#ifndef  CHAT_LOG_H
#define  CHAT_LOG_H

#include <vector>
#include "BaseTypes.h"
#include "ChatMsg.h"

class ChatLog
{
public:
   
   ChatLog();
   
   ui32  append(ChatMsg &msg);
   
   ui32  read(ui32 callerId, ui32 sinceTs, std::vector<ChatMsg*> &msgVec);
   
private:
   
   ui32  m_nCurrentTs;
   
   std::vector<ChatMsg> m_vMsgs;
};

#endif // CHAT_LOG_H