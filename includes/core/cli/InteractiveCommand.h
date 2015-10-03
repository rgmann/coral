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

   //
   // Command processing method invoked when the user enters the associated
   // command name at the prompt.
   //
   virtual void process( const ArgumentList& args ) = 0;

   //
   // Command name accessor.
   //
   const std::string& command() const;

   //
   // Command alias.
   //
   const std::string& alias() const;

   //
   // Check whether this command has an alias.
   //
   bool hasAlias() const;

   //
   // Command description accessor.
   //
   std::string& brief();


private:

   // Command name
   std::string command_;

   // Command description
   std::string brief_;

   // Command alias
   std::string alias_;
};

} // namespace cli
} // namespace liber

#endif // INTERACTIVE_COMMAND_H

