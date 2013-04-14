#ifndef  CHAT_SERVER_LISTENER_H
#define  CHAT_SERVER_LISTENER_H

#include "ServerListener.h"
#include "ChatLog.h"

class ChatServerListener : public ServerListener
{
public:
   
   ChatServerListener();
   
   ~ChatServerListener();
   
protected:
   
   virtual ServerWorker*  createWorker(TcpSocket* pSocket);
   
private:
   
   ChatLog  m_ChatLog;
};

#endif // CHAT_SERVER_LISTENER_H