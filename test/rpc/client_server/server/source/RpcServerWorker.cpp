#include <string.h>
#include <stdio.h>
#include "RpcPacket.h"
#include "RpcServerWorker.h"

//------------------------------------------------------------------------------
RpcServerWorker::RpcServerWorker(TcpSocket* pSocket)
 : ServerWorker(pSocket)
{
   mpRpcServer = NULL;
}

//------------------------------------------------------------------------------
RpcServerWorker::~RpcServerWorker()
{
   
}

//------------------------------------------------------------------------------
void RpcServerWorker::setRpcServer(RpcServer *pRpcServer)
{
   mpRpcServer = pRpcServer;
}

//------------------------------------------------------------------------------
bool RpcServerWorker::processMsg(const char* pMsg, ui32 nMsgLenBytes)
{
   bool       lbSuccess = false;
   RpcPacket* lpPacket = NULL;
   
   ServerWorker::processMsg(pMsg, nMsgLenBytes);
   
   lpPacket = new RpcPacket();
   if (lpPacket == NULL)
   {
      printf("RpcServerWorker::processMsg: Failed to allocate packet\n");
      return false;
   }
   
   lbSuccess = lpPacket->unpack(pMsg, nMsgLenBytes);
   if (!lbSuccess)
   {
      printf("RpcServerWorker: Failed to unpack\n");
      return false;
   }
   
   /*switch (lpPacket->type())
   {
      default:
         pushRx(lpPacket);
         break;
   }*/
   pushRx(lpPacket);
   
   return true;
}

//------------------------------------------------------------------------------
bool RpcServerWorker::work()
{
   RpcPacket* lpRpcInput = NULL;
   RpcPacket* lpRpcOutput = NULL;
   
   // Attempt to receive a packet from the queue.
   if (!popRx((GenericPacket**)&lpRpcInput))
   {
      return false;
   }
   
   //switch (lpPacket->type())
   //{
   //   default:
   mpRpcServer->processPacket(lpRpcInput);
   if (mpRpcServer->popPacket(&lpRpcOutput))
   {
      pushTx(lpRpcOutput);
   }
   //      break;
   //}
   
   return true;
}


//------------------------------------------------------------------------------
bool RpcServerWorker::getMsg(char** pMsg, ui32 &nMsgLenBytes)
{
   bool        lbSuccess = false;
   RpcPacket*  lpRpcPkt = NULL;
   
   lbSuccess = popTx((GenericPacket**)&lpRpcPkt);
   if (!lbSuccess)
   {
      return false;
   }
   
   lbSuccess = lpRpcPkt->pack((void**)pMsg, nMsgLenBytes);
   
   delete lpRpcPkt;
   
   return lbSuccess;
}

//------------------------------------------------------------------------------
ui32 RpcServerWorker::headerSize() const
{
   return sizeof(RpcPacket::Data);
}

//------------------------------------------------------------------------------
ui32 RpcServerWorker::getExpPayloadSize(const char* pHeader) const
{
   const RpcPacket::Data* lpPktHdr = NULL;
   
   lpPktHdr = reinterpret_cast<const RpcPacket::Data*>(pHeader);
   
   if (lpPktHdr == NULL) return 0;
   
   //if (lpPktHdr->marker != ChatPacketHdr::marker) return 0;
   
   return lpPktHdr->length;
}

