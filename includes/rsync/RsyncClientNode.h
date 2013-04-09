#ifndef RSYNC_NODE_H
#define RSYNC_NODE_H

class RsyncNode
{
public:
   
   RsyncNode();
   
   ~RsyncNode();
   
   void  Create(const TcpSocket* pScoket);

private:
   
   void  SegmenterThread(ThreadArg* pArg);
   
   void  AssemblerThread(ThreadArg* pArg);
   
private:
   
   TcpSocket*           m_pSocket;
   
   RsyncSegmenter       m_Segmenter;
   
   RsyncFileAuthority   m_FileAuthority;
   
   RsyncAssembler       m_Assembler;
   
   Thread*  m_pAssemblerThread;
   
   Thread*  m_pSegmenterThread;
};

#endif // RSYNC_NODE_H