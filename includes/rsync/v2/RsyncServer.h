#ifndef RSYNC_HELPER_H
#define RSYNC_HELPER_H


class RsyncServer
{
public:
   
   RsyncNode(unsigned int nSegSizeBytes);
   
   ~RsyncNode();
   
private:
   
   std::ifstream  m_ReadStream;
};

#endif // RSYNC_HELPER_H