#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

#include "BaseTypes.h"
#include "GenericPacket.h"
#include "Mutex.h"
#include "Queue.h"
#include "TcpSocket.h"

class ServerWorker
{
public:
   
   ServerWorker(TcpSocket* pSocket);
   
   ~ServerWorker();
   
   bool  initialize();
   
   TcpSocket* socket(ui32 timeoutMs);
   
   void  releaseSocket();
   
   virtual bool  processMsg(const char* pMsg, ui32 nMsgLenBytes) = 0;
   
   virtual bool  work() = 0;
   
   virtual bool  getMsg(char** pMsg, ui32 &nMsgLenBytes) = 0;
   
   virtual ui32  headerSize() const = 0;
   
   virtual ui32  getExpPayloadSize(const char* pHeader) const = 0;
   
protected:
   
   void pushRx(GenericPacket*);
   bool popRx(GenericPacket**);
   
   void pushTx(GenericPacket*);
   bool popTx(GenericPacket**);
   
protected:
   
   Mutex m_SocketLock;
   
   TcpSocket*  m_pSocket;
   
   // ProcessMsg parses and validates the received packet.
   // If the packet is valid it adds it to the IN queue;
   Queue<GenericPacket*> m_InQ;
   
   // The work method adds packed messages to the output queue.
   Queue<GenericPacket*> m_OutQ;
};

#endif // SERVER_WORKER_H