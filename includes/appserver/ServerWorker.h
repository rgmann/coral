#ifndef SERVER_WORKER_H
#define SERVER_WORKER_H

class ServerWorker
{
public:
   
   ServerWorker(TcpSocket* pSocket);
   
   ~ServerWorker();
   
   TcpSocket* socket(ui32 timeoutMs);
   
   void  releaseSocket();
   
   virtual bool  processMsg(const char* pMsg, ui32 nMsgLenBytes) = 0;
   
   virtual bool  work() = 0;
   
   virtual bool  getMsg(char** pMsg, ui32 &nMsgLenBytes) = 0;
   
   virtual ui32  headerSize() const = 0;
   
   virtual ui32  getExpPayloadLen(const char* pHeader) = 0;
   
private:
   
   Mutex m_SocketLock;
   
   TcpSocket*  m_pSocket;
   
   // ProcessMsg parses and validates the received packet.
   // If the packet is valid it adds it to the IN queue;
   Queue<char*> m_InQ;
   
   // The work method adds packed messages to the output queue.
   Queue<char*> m_OutQ;
};

#endif // SERVER_WORKER_H