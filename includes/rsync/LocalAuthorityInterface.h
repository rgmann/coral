#ifndef RSYNC_LOCAL_AUTHORITY_INTERFACE_H
#define RSYNC_LOCAL_AUTHORITY_INTERFACE_H

#include <fstream>
#include <boost/filesystem.hpp>
#include "Authority.h"

namespace liber {
namespace rsync {

class Segment;
class FileSystemInterface;
class RsyncJob;

class LocalAuthorityInterface {
public:

  explicit LocalAuthorityInterface( FileSystemInterface& );
  ~LocalAuthorityInterface();

  void process( RsyncJob* job_ptr );
  void process( RsyncJob* job_ptr, InstructionReceiver& instruction_receiver );

private:

  LocalAuthorityInterface( const LocalAuthorityInterface& );
  LocalAuthorityInterface& operator= ( const LocalAuthorityInterface& );

  void processJob(
    RsyncJob* job_ptr,
    InstructionReceiver& instruction_receiver
  );

  bool open(const boost::filesystem::path& path);
  void close();
  std::ifstream& file();

private:

  int segment_timeout_ms_;

  FileSystemInterface& file_sys_interface_;
  std::ifstream authoritative_file_;

  Authority authority_;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_LOCAL_AUTHORITY_INTERFACE_H
