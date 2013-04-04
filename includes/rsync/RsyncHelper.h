#ifndef RSYNC_HELPER_H
#define RSYNC_HELPER_H

#include <vector>
#include <string>
#include "RsyncSegment.h"

// Only methods that are common to the server and client
class RsyncHelper
{
public:
   
   enum WeakPolicy
   {
      DoNotRoll,
      DoRoll
   };
   
   enum StrongPolicy
   {
      DeferStrongComp,
      DoStrongComp
   };
   
   static bool SegmentFile(const std::string &rootPath,
                           const std::string &relPath,
                           std::vector<RsyncSegment*> &segments,
                           unsigned int   nSegLenBytes,
                           unsigned int   nStrideBytes,
                           WeakPolicy  weakPol,
                           StrongPolicy strongPol);
};

#endif // RSYNC_HELPER_H