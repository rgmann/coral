#ifndef RSYNC_FILE_STAGE_H
#define RSYNC_FILE_STAGE_H

#include <string>
#include <fstream>
#include "BaseTypes.h"

class RsyncFileStage
{
public:
   
   RsyncFileStage();
   
   ~RsyncFileStage();
   
   bool open(const std::string &fullFilename);
   
   bool add(ui8* pChunk, ui32 nChunkSizeBytes);
   
   bool swap();
   
   void close();
   
   std::ifstream& stream();
   
private:
   
   static std::string GenerateStageFilename(const std::string &filename);
   
private:
   
   std::string    m_sLiveFilename;
   
   std::string    m_sStageFilename;
   
   std::ofstream  m_ofsAssemblyStage;
   
   std::ifstream  m_ifsLiveFile;
};

#endif // RSYNC_FILE_STAGE_H