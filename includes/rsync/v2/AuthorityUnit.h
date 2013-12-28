#ifndef AUTHORITY_UNIT_H
#define AUTHORITY_UNIT_H

#include <string>
#include "WorkUnit.h"
#include "RsyncFileAuthority.h"

namespace liber {
namespace rsync {

class AuthorityUnit : public WorkUnit {
public:
   
  AuthorityUnit(Queue<RsyncPacket*>& outQueue);

  ~AuthorityUnit();

  void  setRoot(const std::string& root);

  virtual bool work();
   
private:

   RsyncSegmentTable*   m_pSegmentTable;

   RsyncFileAuthority   m_authority;
};

}}

#endif // AUTHORITY_UNIT_H
