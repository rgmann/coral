#ifndef ASSEMBLY_BEGIN_MARKER_H
#define ASSEMBLY_BEGIN_MARKER_H

#include <string>
#include "GenericPacket.h"

namespace liber {
namespace rsync {

class AssemblyBeginMarker : public liber::netapp::GenericPacket {
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
   
   bool setPath(const std::string &filepath);
   
   // Empty string indicates an error.
   std::string getPath() const;
   
   bool  unpack(const void* pPkt, ui32 nSizeBytes);
   
private:

   typedef liber::netapp::GenericPacket inherited;
};

}}

#endif // ASSEMBLY_BEGIN_MARKER_H
