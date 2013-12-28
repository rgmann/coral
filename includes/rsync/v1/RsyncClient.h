#ifndef RSYNC_ClIENT_H
#define RSYNC_ClIENT_H

struct RsyncClientStatus
{
   bool  bServerConnected;
   bool  bServerAccepted;
   
   unsigned int nSegmentCount;
   unsigned int nSegmentSize;
   
   unsigned int nSegmentsTx;
   unsigned int nSegmentRx;
   
   // Chunks are aribtrarily sized blocks of data received from the server.
   // Chunks contain data found in the authoritative file that are missing
   // from the client file.
   unsigned int nChunksRx;
   
   bool  bDone;
   
   bool  bStagingEnabled;
   unsigned int nStageThreshold;
   
   // Pre-processing, cancelled, reconstructing, etc.
   RsyncClientState state;
}

typedef void (RsyncComplCb*)(const RsyncClientStatus &status);

class RsyncClient
{
public:
   
   RsyncClient();
   
   ~RsyncClient();
   
   void  setSocket(Socket *pSocket);
   
   bool  start(const std::string &filename);
   
   bool  cancel();
   
   void  subscribeCompletion(RsyncComplCb pCb);
   
   unsigned int getSegmentCount();
   
private:
   
   // TODO: Override copy construct and = operator
   
   void  segmenterThread(Thread* pThread);
   
   void  rebuilderThread(Thread* pThread);
   
private:
   
   std::ifstream  m_File;
   
   Thread*  m_pSegmenter;
   
   Thread*  m_pRebuilder;
   
   Socket*  m_pSocket;
};


#endif // RSYNC_ClIENT_H