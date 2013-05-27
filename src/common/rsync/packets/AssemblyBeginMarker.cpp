#include "AssemblyBeginMarker.h"

//------------------------------------------------------------------------------
AssemblyBeginMarker::AssemblyBeginMarker() : GenericPacket()
{
}

//------------------------------------------------------------------------------
AssemblyBeginMarker::AssemblyBeginMarker(const std::string &filepath)
: GenericPacket()
{
   setPath(filepath);
}

//------------------------------------------------------------------------------
AssemblyBeginMarker::~AssemblyBeginMarker()
{
   // Nothing to destroy
}

//------------------------------------------------------------------------------
bool AssemblyBeginMarker::setPath(const std::string &filepath)
{
   bool l_bSuccess = false;
   
   deallocate();
   
   m_nSizeBytes = filepath.length();
   
   if (l_bSuccess = allocate())
   {
      strncpy((char*)basePtr(), filepath.c_str(), allocatedSize());
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool AssemblyBeginMarker::getPath(std::string &filepath)
{
   bool l_bSuccess = false;
   filepath = "";
   
   if (isAllocated())
   {
      char* l_pFilepath = reinterpret_cast<char*>(dataPtr());
      filepath = std::string(l_pFilepath);
      l_bSuccess = true;
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
ui32 AssemblyBeginMarker::dataSize() const
{
   return allocatedSize();
}

//------------------------------------------------------------------------------
//bool AssemblyBeginMarker::unpack(const void* pPkt, ui32 nSizeBytes)
//{
//   ui32 l_nDataSize = 0;
//   
//   if (!inherited::unpack(pPkt, nSizeBytes))
//   {
//      printf("AssemblyBeginMarker::unpack: inherited unpack failed.\n");
//      return false;
//   }
//   
//   // Verify that the indicated packet size is the same as the
//   // actual packet size.
//   if (l_nDataSize != (inclusiveSize() - dataOffset()))
//   {
//      printf("AssemblyBeginMarker::unpack: incorrect size\n");
//      return false;
//   }
//   
//   return true;
//}
