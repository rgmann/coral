// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 



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