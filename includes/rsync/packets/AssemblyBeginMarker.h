#ifndef ASSEMBLY_BEGIN_MARKER_H
#define ASSEMBLY_BEGIN_MARKER_H

#include <string>
#include "GenericPacket.h"

class AssemblyBeginMarker : public GenericPacket
{
public:
   
   enum Type
   {
   };
   
   struct __attribute__((__packed__)) Data
   {
      // Filepath
   };
   
   
   AssemblyBeginMarker();
   
   /**
    * 
    */
   AssemblyBeginMarker(const std::string &filepath);
   
   ~AssemblyBeginMarker();
   
   bool  setPath(const std::string &filepath);
   
   bool  getPath(std::string &filepath);
   
   //bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
protected:
   
   virtual ui32 dataSize() const;

private:

   typedef GenericPacket inherited;
};

#endif // ASSEMBLY_BEGIN_MARKER_H