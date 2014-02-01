#ifndef INTERACTIVE_COMMAND_H
#define INTERACTIVE_COMMAND_H

#include <string>
#include <vector>

namespace liber {
namespace cli {

typedef std::vector<std::string> ArgumentList;

class InteractiveCommand {
public:

  InteractiveCommand(const std::string& command,
                     const std::string& brief = "",
                     const std::string& alias = "");
  virtual ~InteractiveCommand();

  virtual void process(const ArgumentList& args) = 0;

  const std::string& command() const;

  const std::string& alias() const;
  bool hasAlias() const;

  std::string& brief();

private:

  std::string mCommand;
  std::string mBrief;
  std::string mAlias;
};

}}

#endif // INTERACTIVE_COMMAND_H

