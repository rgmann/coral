#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <vector>
#include <map>
#include "Argument.h"

namespace liber {
namespace cli {

class ArgParser {
public:
   
   ArgParser();
   
   ~ArgParser();
   
   bool  addArg( const std::string &arg );
   
   bool  parse( const char* argv[], int argc );
   
   void  printArgErrors(bool bPrinHelp = false);
   
   void  printHelp();
   
   bool  isSet( Argument::ArgField field, const std::string &fieldStr );
   
   bool  getArgVal(int &val, 
                   Argument::ArgField field, 
                   const std::string &fieldStr);
   bool  getArgVal(float &val, 
                   Argument::ArgField field, 
                   const std::string &fieldStr);
   bool  getArgVal(std::string &val, 
                   Argument::ArgField field, 
                   const std::string &fieldStr);
   
private:
   
   bool getArgByField(Argument** pArgInfo, 
                      Argument::ArgField field, 
                      const std::string &fieldStr);
   
   void  logError(const std::string &optId, const std::string &msg);
   void  resetErrorLog();
   int   errorCount();
   
   void  resetArgs();
   
   static bool ValidType(Argument::ArgValType type, const std::string &value);
   
   void printVec(const std::string &title, const std::vector<Argument*> &vec);
   void printMap(const std::string &title, const std::map<std::string, Argument*> &map);
   
private:
   
   std::map<std::string,Argument*>  primary_table_;
   std::map<std::string,Argument*>  alt_table_;
   std::map<std::string,Argument*>  name_table_;
   std::vector<Argument*>           arguments_;
   std::vector<std::string>         argument_errors_;
};

} // namespace cli
} // namespace liber

#endif // ARG_PARSER_H