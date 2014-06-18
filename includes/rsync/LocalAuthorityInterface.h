#ifndef RSYNC_LOCAL_AUTHORITY_INTERFACE_H
#define RSYNC_LOCAL_AUTHORITY_INTERFACE_H

#include <fstream>
#include "HashTable.h"
//#include "AuthorityInterface.h"
//#include "FileSystemInterface.h"
#include "Authority.h"

namespace liber {
namespace rsync {

class Segment;
class FileSystemInterface;
class RsyncJob;
//class InstructionReceiver;

//class LocalAuthorityInterface : public AuthorityInterface {
class LocalAuthorityInterface {
public:

  explicit LocalAuthorityInterface(FileSystemInterface&);
  ~LocalAuthorityInterface();

  void process(RsyncJob* pJob);
  void process(RsyncJob* pJob, InstructionReceiver& rInstructions);

private:

  void processJob(RsyncJob* pJob, InstructionReceiver& rInstructions);

  bool open(const boost::filesystem::path& path);
  void close();
  std::ifstream& file();

private:

  int mnSegmentTimeoutMs;

  FileSystemInterface& mrFileSys;
  std::ifstream mAuthFile;

//  liber::HashTable<16, Segment*> mHash;
  Authority mAuthority;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_LOCAL_AUTHORITY_INTERFACE_H
