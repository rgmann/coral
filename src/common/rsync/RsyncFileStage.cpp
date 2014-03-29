#include <iostream>
#include "RsyncFileStage.h"
#include "FileSystemHelper.h"
#include <stdio.h>

//------------------------------------------------------------------------------
RsyncFileStage::RsyncFileStage()
{
   m_sLiveFilename = "";
   m_sStageFilename = "";
}

//------------------------------------------------------------------------------
RsyncFileStage::~RsyncFileStage()
{
}

//------------------------------------------------------------------------------
bool RsyncFileStage::open(const std::string &fullFilename)
{
   m_sLiveFilename = fullFilename;
   m_sStageFilename = GenerateStageFilename(m_sLiveFilename);
   
   printf("RsyncFileStage::open: stage file name = %s\n",
          m_sStageFilename.c_str());
   
   m_ifsLiveFile.open(m_sLiveFilename.c_str(), std::ios::binary);
   if (!m_ifsLiveFile.good())
   {
      return false;
   }
   
   m_ofsAssemblyStage.open(m_sStageFilename.c_str(), std::ios::binary);
   if (!m_ofsAssemblyStage.good())
   {
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool RsyncFileStage::add(ui8* pChunk, ui32 nChunkSizeBytes)
{
   if (!m_ofsAssemblyStage.is_open())
   {
      return false;
   }
   
   m_ofsAssemblyStage.write((char*)pChunk, nChunkSizeBytes);
   
   return m_ofsAssemblyStage.good();
}

//------------------------------------------------------------------------------
bool RsyncFileStage::swap()
{
   if (m_ofsAssemblyStage.is_open())
   {
      // Close the file before performing the swap
      m_ofsAssemblyStage.close();
   }
   
   if (m_ifsLiveFile.is_open())
   {
      // Close the file before performing the swap
      m_ifsLiveFile.close();
   }
   
   printf("RsyncFileStage::swap\n");
   
   return FileSystemHelper::Move(m_sStageFilename, m_sLiveFilename, true);
}

//------------------------------------------------------------------------------
void RsyncFileStage::close()
{
   m_ifsLiveFile.close();
   m_ofsAssemblyStage.close();
}

//------------------------------------------------------------------------------
std::ifstream& RsyncFileStage::stream()
{
   return m_ifsLiveFile;
}

//------------------------------------------------------------------------------
std::string RsyncFileStage::GenerateStageFilename(const std::string &fullPath)
{
   size_t      l_nPosLastSlash = 0;
   std::string filename = "";
   std::string l_sRootName = "";
   
   l_nPosLastSlash   = fullPath.rfind("/");
   filename = fullPath.substr(l_nPosLastSlash + 1, std::string::npos);
   filename = ("." + filename + ".stage");
   l_sRootName = fullPath.substr(0, l_nPosLastSlash + 1);
   
   return FileSystemHelper::Concat(l_sRootName, filename);
}