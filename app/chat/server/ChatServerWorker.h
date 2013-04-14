#ifndef  CHAT_SERVER_WORKER_H
#define  CHAT_SERVER_WORKER_H

#include "ServerWorker.h"
#include "ChatLog.h"

class ChatServerWorker : public ServerWorker
{
public:
   
   ChatServerWorker(TcpSocket* pSocket);
   
   ~ChatServerWorker();
   
   void  setChatLog(ChatLog *pLog);
   
   bool  processMsg(const char* pMsg, ui32 nMsgLenBytes);
   
   bool  work();
   
   bool  getMsg(char** pMsg, ui32 &nMsgLenBytes);
   
   virtual ui32  headerSize() const;
   
   virtual ui32  getExpPayloadSize(const char* pHeader) const;
   
protected:
   
   ChatLog* m_pChatLog;
};

#endif // CHAT_SERVER_WORKER_H