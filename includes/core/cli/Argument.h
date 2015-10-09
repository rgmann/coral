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



#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>

namespace liber {
namespace cli {

class Argument {
public:
   
   enum ArgType {
      ArgFlag,
      ArgOption
   };
   
   enum ArgValType {
      ArgInt,
      ArgString,
      ArgFloat
   };
   
   enum ArgField {
      ArgPrimary,
      ArgAlt,
      ArgName
   };
   
   enum ArgPolicy {
      ArgOptional,
      ArgRequired
   };
   
   /**
    * Default constructor.
    */
   Argument();
   
   /**
    * Create string from string definition.
    * String definitions in Ruby Hash-style format.
    *
    * ex.: "primary: v, verbose: verbose, name: verbose, type: flag, optional"
    * ex.: "primary: p, verbose: port, name: port, type: option, default: 5000"
    */
   static Argument*   Create(const std::string &definition);
   
   bool  fromString(const std::string &definition);
   
   void  setId(ArgField field, const std::string &value);
   
   void        name(const std::string &val){ m_sName = val; };
   std::string name(){ return m_sName; };
   
   void        primary(const std::string &val){ m_sId = val; };
   std::string primary(){ return m_sId; };
   
   void        alt(const std::string &val){ m_sAltId = val; };
   std::string alt(){ return m_sAltId; };
   bool        hasAlt() const { return ( m_sAltId.empty() == false ); }
   
   void        description(const std::string &val){ m_sDescription = val; };
   std::string description(){ return m_sDescription; };
   
   void        policy(ArgPolicy val){ m_Policy = val; };
   ArgPolicy   policy(){ return m_Policy; };
   
   void        type(ArgType val){ m_Type = val; };
   ArgType     type(){ return m_Type; };
   
   void        value(const std::string &val){ m_sValue = val; };
   std::string value(){ return m_sValue; };
   void        clearValue(){ m_sValue.erase(); };
   
   void        valtype(ArgValType val){ m_ValType = val; };
   ArgValType  valtype(){ return m_ValType; };
   
private:
   
   enum ArgStatus {
      ArgOmitted,
      ArgMissingParam,
      ArgBadValue,
      ArgFound
   };
   
   
   bool  parse(const std::string &definition);
      
   void        status(ArgStatus val){ m_Status = val; };
   ArgStatus   status(){ return m_Status; };
   
   bool  setAttribute(const std::string &attrName, 
                      const std::string &attrVal = "");
   
private:
   
   std::string m_sName;
   std::string m_sId;
   std::string m_sAltId;
   std::string m_sDescription;
   ArgType     m_Type;
   ArgPolicy   m_Policy;
   ArgValType  m_ValType;
   
   std::string m_sValue;
   std::string m_sDefault;
   ArgStatus   m_Status;
   
   friend class ArgParser;
};

} // namespace cli
} // namespace liber

#endif // ARGUMENT_H