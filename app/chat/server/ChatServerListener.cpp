#include "ChatServerWorker.h"
#include "ChatServerListener.h"

//------------------------------------------------------------------------------
ChatServerListener::ChatServerListener() : ServerListener()
{
}

//------------------------------------------------------------------------------
ChatServerListener::~ChatServerListener()
{
}

//------------------------------------------------------------------------------
ServerWorker* ChatServerListener::createWorker(TcpSocket* pSocket)
{
   ChatServerWorker* l_pWorker = new ChatServerWorker(pSocket);
   
   if (l_pWorker)
   {
      l_pWorker->setChatLog(&m_ChatLog);
   }
   
   return l_pWorker;
}
