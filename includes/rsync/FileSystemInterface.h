#ifndef RSYNC_FS_INTERFACE_H
#define RSYNC_FS_INTERFACE_H

#include <boost/filesystem.hpp>

namespace liber {
namespace rsync {

class FileSystemInterface {
public:

  FileSystemInterface(const boost::filesystem::path& swapPath = ".rsync_swap",
                      const boost::filesystem::path& stagePath = ".rsync_stage");
  ~FileSystemInterface();

  bool setRoot(const boost::filesystem::path& root);

  bool exists(const boost::filesystem::path& path) const;

  bool open(const boost::filesystem::path& path, std::ofstream& stream) const;
  bool open(const boost::filesystem::path& path, std::ifstream& stream) const;

  bool stage(const boost::filesystem::path& destination,
             boost::filesystem::path& stagePath,
             std::ofstream& stream);

  bool swap(const boost::filesystem::path& source,
            const boost::filesystem::path& destination);

  bool remove(const boost::filesystem::path& path) const;
  bool touch(const boost::filesystem::path& path) const;

private:

  FileSystemInterface(const FileSystemInterface&);
  FileSystemInterface& operator= (const FileSystemInterface&);

  bool validate() const;

  /**
   * TODO: Allowed?  This could be used to find out base path.
   */
  boost::filesystem::path absolutePath(const boost::filesystem::path& relativePath) const;

private:

  bool mbRootExists;

  boost::filesystem::path mSwapPath;

  boost::filesystem::path mStagePath;

  // Absolute path to rsync root directory.  Rsync does not know about
  // anything outside this path.
  boost::filesystem::path mRootPath;

  int mnLastSwapFileIndex;
  int mnLastStageFileIndex;
};

} // End namespace rsync
} // End namespace liber

#endif // RSYNC_FS_INTERFACE_H
