#ifndef RSYNC_SERVER_H
#define RSYNC_SERVER_H

#include "FileQueryAgent.h"

namespace liber {
namespace rsync {

class RsyncServer : public liber::netapp::PacketSubscriber {
public:
   
   RsyncServer();
   
   ~RsyncServer();

  void setFileQueryAgent(FileQueryAgent& rQueryAgent);

private:

  FileQueryAgent  mDefaultQuery;
  FileQueryAgent& mrFileQuery;

};

}}

#endif // RSYNC_SERVER_H
