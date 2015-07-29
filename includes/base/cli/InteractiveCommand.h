#ifndef INTERACTIVE_COMMAND_H
#define INTERACTIVE_COMMAND_H

#include <string>
#include <vector>

namespace liber {
namespace cli {

typedef std::vector<std::string> ArgumentList;

//
// InteractiveCommand abstract base class.
//
class InteractiveCommand {
public:

  InteractiveCommand(const std::string& command,
                     const std::string& brief = "",
                     const std::string& alias = "");
  virtual ~InteractiveCommand();

  virtual void process( const ArgumentList& args ) = 0;

  const std::string& command() const;

  const std::string& alias() const;
  bool hasAlias() const;

  std::string& brief();

private:

  std::string command_;
  std::string brief_;
  std::string alias_;
};

}}

#endif // INTERACTIVE_COMMAND_H

