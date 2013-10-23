// Description:
// The client sends this message when it needs to synchronize the local copy of
// a file to the master copy.  Additionally, when the client receives a 
// CREATE file message, it creates an empty file and then sends a read file
// message to the server.  The read file message is acknowledged and then
// the client initiates the rsync stream.

#ifndef CB_READ_FILE_MSG_H
#define CB_READ_FILE_MSG_H

#include "GenericPacket.h"

class CbReadFileMsg : public GenericPacket
{
public:

   struct __attribute ((__packed__)) Data
   {
      // Filename
   };

   CbReadFileMsg();

   CbReadFileMsg(const std::string &path);

   bool  unpack(void* pPkt, ui32 nSizeBytes);

private:

   typedef GenericPacket inherited;

};

#endif // CB_READ_FILE_MSG_H
