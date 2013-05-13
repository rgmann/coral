#include <stdio.h>
#include <string.h>
#include <iostream>
#include <signal.h>
#include "ArgParser.h"
#include "TcpSocket.h"
#include "TcpClient.h"
#include "Thread.h"
#include "Queue.h"
#include "ChatMsg.h"
#include "ChatMsgPacket.h"
#include "UpdateRequestPacket.h"
#include "UpdateResponsePacket.h"

bool  g_bStopCommanded = false;


void sighandler(int s)
{
   g_bStopCommanded = true;
}

void rsyncThread(ThreadArg* pArg);
void socketThread(ThreadArg* pArg);
i32  receiveRespUpdate(UpdateResponsePacket** pResp, TcpSocket* pSocket);

struct SocketThreadParams
{
   std::string hostName;
   ui32        port;
   Queue<RsyncPacket*>* pTxQueue;
   Queue<RsyncPacket*>* pAssemblerRxQueue;
   Queue<RsyncPacket*>* pSegmenterRxQueue;
};

struct AssemblerThreadParams
{
   //std::string path;
   RsyncSegmenter*      pSegmenter;
   Queue<RsyncPacket*>* pTxQueue;
   Queue<RsyncPacket*>* pRxQueue;
};

struct SegmenterThreadParams
{
   std::string          root;
   std::string          path;
   Queue<RsyncPacket*>* pTxQueue;
   Queue<RsyncPacket*>* pRxQueue;
};

int main(int argc, char *argv[])
{
   static const ui32 HostPort = 14001;
   static const std::string HostName = "192.168.1.116";
   
   RsyncSegmenter          segmenter;
   SocketThreadParams      socketParams;
   AssemblerThreadParams   assemblerParams;
   SegmenterThreadParams   segmenterParams;
   
   ArgParser args;
   Thread*  l_pSocketThread = NULL;
   Thread*  l_pSegmenterThread = NULL;
   Thread*  l_pAssemblerThread = NULL;
   
   Queue<RsyncPacket*> txQueue;
   Queue<RsyncPacket*> assemblerRxQueue;
   Queue<RsyncPacket*> segmenterRxQueue;
   
   struct sigaction sigIntHandler;
      
   printf("Syncr Client:\n\n");
   
//   args.addArg("name: HostName, primary: h, alt: host, type: opt, "
//               "vtype: string, required, desc: Host name/address");
//   
//   args.addArg("name: Port, primary: p, alt: port, type: opt, "
//               "vtype: int, required, desc: Set port number");
   
   args.addArg("name: Path, primary: p, alt: path, type: opt, "
               "vtype: string, required, desc: Full/relative path");
   
   args.addArg("name: Method, primary: m, alt: method, type: opt, "
               "vtype: string, required, desc: PUSH/PULL");
   
   if (!args.parse((const char**)argv, argc))
   {
      args.printArgErrors(true);
      return 1;
   }
   
//   args.getArgVal(g_sHostName, Argument::ArgName, "HostName");
//   args.getArgVal(g_nPort, Argument::ArgName, "Port");
   args.getArgVal(rsyncParams.path, Argument::ArgName, "Path");
   //args.getArgVal(l_sMethod, Argument::ArgName, "Method");
   
   sigIntHandler.sa_handler = sighandler;
   sigemptyset(&sigIntHandler.sa_mask);
   sigIntHandler.sa_flags = 0;
   sigaction(SIGINT, &sigIntHandler, NULL);
   
   rxQueue.initialize(10);
   txQueue.initialize(10);
   fwdQueue.initialize(10);
   
   assemblerParams.pRxQueue = &rxQueue;
   assemblerParams.pTxQueue = &txQueue;
   assemblerParams.pRxQueue = &rxQueue;
   
   segmenterParams.pRxQueue = &rxQueue;
   segmenterParams.pTxQueue = &txQueue;
   
   socketParams.hostName   = HostName;
   socketParams.port       = HostPort;
   socketParams.pRxQueue   = &rxQueue;
   socketParams.pTxQueue   = &txQueue;
   
   
   l_pSocketThread = Thread::Create(socketThread, &socketParams);
   if (l_pSocketThread == NULL)
   {
      printf("Failed to start RX thread.\n");
      return 1;
   }
   
   
   l_pAssemblerThread = Thread::Create(assemblerThread, &assemblerParams);
   if (l_pSocketThread == NULL)
   {
      printf("Failed to start RX thread.\n");
      return 1;
   }
   
   
   l_pSegmenterThread = Thread::Create(segmenterThread, &segmenterParams);
   if (l_pSocketThread == NULL)
   {
      printf("Failed to start RX thread.\n");
      return 1;
   }
   
   while (!g_bStopCommanded)
   {
      
   }
   
   printf("Stopping\n");
   
   if (l_pSocketThread)
   {
      l_pSocketThread->stop();
      l_pSocketThread->join();
   }
   
   return 0;
}

//------------------------------------------------------------------------------
void assemblerThread(ThreadArg* pArg)
{
   RsyncAssembler       assembler;
   
   assembler.setSegmenter(pParams->pSegmenter);
   
   while (!pArg->stopSignalled())
   {
   }
}

//------------------------------------------------------------------------------
void segmenterThread(ThreadArg* pArg)
{
   SegmenterThreadParams* pParams = NULL;
   
   RsyncSegmentReport*  pReport = NULL;
   RsyncReportHeader    reportHeader;
   RsyncPackedSeg       packedSeg;
   
   
   RsyncSegmentTable    table;
   int                  l_nSegCount = 0;
   
   RsyncFileAuthority   authority;
   
   RsyncAssembler       assembler;
   
   
   pParams = (SegmenterThreadParams*)pArg->pUserData;
   
   segmenter.setRoot(pParams->root);
   segmenter.setSegmentSize(6);
   
   if (!segmenter.process(pParams->path))
   {
      std::cout << "Failed to segment client file." << std::endl;
      return 1;
   }
   
//   if (!sendFileQuery(pParams->root, pParams->path))
//   {
//      std::cout << "Failed to send file query." << std::endl;
//      return;
//   }
//   
//   pFileQueryResp = waitFileQueryResponse(1000);
//   if (pFileQueryResp == NULL)
//   {
//      std::cout << "Timed out receiving file query response." << std::endl;
//      return
//   }
//   
//   pFileQueryResp
   
   // Once processing is complete, we can retrieve the segmenation report.
   pReport = segmenter.getReport(filename);
   if (pReport == NULL)
   {
      std::cout << "Failed to get Segmentation Report" << std::endl;
      return 1;
   }
   
   while (pReport->getNextSegment(pSegment))
   {
      while (!pParams->txQueue(pSegment, 100));
   }
   
   // TODO: Signal complete
   
   while (!pArg->stopSignalled())
   {
   }
}

//------------------------------------------------------------------------------
void socketThread(ThreadArg* pArg)
{
   SocketThreadParams*  l_pParams     = NULL;
   RsyncPacket*         l_pRsyncPkt = NULL;
   TcpSocket*  l_pSocket = NULL;
   ui32           l_nCurrentTs = 0;
   
   l_pParams = (SocketThreadParams*)pArg->pUserData;
   
   // Connect to server
   l_pSocket = TcpClient::Connect(l_pParams->hostName, l_pParams->port, 2000);
   
   if (l_pSocket == NULL)
   {
      printf("Failed to connect to server\n");
      while (!pArg->stopSignalled()) sleep(1);
   }
   
   while (!pArg->stopSignalled())
   {      
      // Check for a new msg in the tx queue
      if (pParams->txQueue.pop(l_pRsyncPkt, 100))
      {
         char*          l_pPktData = NULL;
         ui32           l_nPktLen = 0;
         
         l_pRsyncPkt->pack((void**)&l_pPktData, l_nPktLen);
         
         l_pSocket->send(l_pPktData, l_nPktLen);
         
         // Delete the packed version of the packet
         delete[] l_pPktData;
         l_pPktData = NULL;
         
         // Delete the packet instance received from the queue
         delete l_pRsyncPkt;
         l_pRsyncPkt = NULL;
      }
      
      // Attempt to receive a packet and push it onto the RX queue
      if (receiveRespUpdate(l_pRsyncPkt, l_pSocket, 100))
      {
         routePacket(l_pRsyncPkt,
                     pParams->pSegmenterRxQ,
                     pParams->pAssemblerRxQ);
         
         l_pRsyncPkt = NULL;
      }
   }
   
   l_pSocket->closeSocket();
}

//------------------------------------------------------------------------------
bool receiveRespUpdate(RsyncPacket** pPkt, TcpSocket* pSocket, ui32 timeoutMs)
{
   RsyncPacketHeader  header;
   bool           l_bSuccess = true;
   i32           l_nBytesRecvd = 0;
   char*          l_pPkt = NULL;
   ui32           l_nDataLen     = 0;

   
   // Attempt to receive a chat packet header
   l_nBytesRecvd = pSocket->recv((char*)&header,
                                 sizeof(RsyncPacketHeader),
                                 timeoutMs);
   
   if (l_nBytesRecvd == -1)
   {
      return false;
   }
   
   if (header.marker != RsyncPacketHeader::marker) return false;
   
   // Determine how much data is included in the packet.
   l_nDataLen = header.length;
   l_pPkt = new char[sizeof(ChatPacketHdr) + l_nDataLen];
   
   if (!l_pPkt)
   {
      return false;
   }
   
   // Copy the previously read header into the allocated packet.
   memcpy(l_pPkt, &header, sizeof(RsyncPacketHeader));
   
   // Receive the rest of the packet directly into the allocated buffer.
   l_nBytesRecvd = pSocket->recv(l_pPkt + sizeof(ChatPacketHdr),
                                 l_nDataLen, 1000);
   
   if (l_nBytesRecvd == l_nDataLen)
   {
      *pPkt = new RsyncPacket();
      (*pPkt)->unpack(l_pPkt, sizeof(RsyncPacketHeader) + l_nDataLen);
   }
   else
   {
      l_bSuccess = false;
   }
   
   if (l_pPkt)
   {
      delete l_pPkt;
      l_pPkt = NULL;
   }
   
   return l_bSuccess;
}
