#ifndef RSYNC_PACKET_SUBSCRIBER_H
#define RSYNC_PACKET_SUBSCRIBER_H


namespace liber {
namespace rsync {

class AuthorityCollection : public netapp::PacketSubScriber {
public:

  AuthorityCollection();
  ~AuthorityCollection();

  bool put(const char* pData, ui32 nLength);

private:

  std::map<AuthorityID, Authority*> mAuthorityJobs;

};

}}

#endif // RSYNC_PACKET_SUBSCRIBER_H

