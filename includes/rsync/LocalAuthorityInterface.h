#ifndef RSYNC_LOCAL_AUTHORITY_INTERFACE_H
#define RSYNC_LOCAL_AUTHORITY_INTERFACE_H

#include <fstream>
#include "HashTable.h"
#include "AuthorityInterface.h"
//#include "FileSystemInterface.h"

namespace liber {
namespace rsync {

class Segment;
class FileSystemInterface;

class LocalAuthorityInterface : public AuthorityInterface {
public:

  explicit LocalAuthorityInterface(FileSystemInterface&);
  ~LocalAuthorityInterface();

private:

  void processJob(RsyncJob* pJob);

  bool open(const boost::filesystem::path& path);
  void close();
  std::ifstream& file();

private:

  int mnSegmentTimeoutMs;

  FileSystemInterface& mrFileSys;
  std::ifstream mAuthFile;

  liber::HashTable<16, Segment*> mHash;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_LOCAL_AUTHORITY_INTERFACE_H
