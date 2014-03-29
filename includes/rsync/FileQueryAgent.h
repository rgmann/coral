#ifndef FILE_QUERY_AGENT_H
#ifndef FILE_QUERY_AGENT_H

namespace liber {
namespace rsync {

class FileQueryAgent {
public:

  FileQueryAgent();
  virtual ~FileQueryAgent();

  virtual bool exists(const FilePath& path);

  virtual bool getInputStream(const FilePath& path, std::ifstream& stream);

  virtual bool getOutputStream(const FilePath& path, std::ofstream& stream);

};

}}

#endif // FILE_QUERY_AGENT_H

