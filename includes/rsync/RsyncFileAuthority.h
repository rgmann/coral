#ifndef RSYNC_FILE_AUTHORITY_H
#define RSYNC_FILE_AUTHORITY_H

#include "RsyncCommon.h"
#include "RsyncAssemblyInstr.h"
#include "RsyncSegmentTable.h"

#include <string>

struct RsyncAssemblySeqMarker
{
   static const unsigned int marker;
   char fullFileName[RsyncMaxPathLength];
};

class RsyncFileAuthority
{
public:
   
   RsyncFileAuthority();
   
   ~RsyncFileAuthority();
   
   void setRoot(const std::string &root);
   
   bool process(RsyncSegmentTable* pSegTable);
   
   bool getNextInstruction(RsyncAssemblyInstr **pInst);
   
   void cleanup();

private:
   
   bool createBeginMarker(const std::string &relFilePath);
   
   bool createEndMarker();
   
   bool createSegmentInstruction(RsyncSegId segmentId);
   
   bool createChunkInstruction(std::ifstream &file,
                                unsigned int nOffsetBytes,
                                unsigned int nSizeBytes);
   
private:
   
   std::string m_sRootDir;
   
   std::vector<RsyncAssemblyInstr*> m_vInstrSeq;
   std::vector<RsyncAssemblyInstr*>::iterator m_iInstrSeqIt;
};

#endif // RSYNC_FILE_AUTHORITY_H