#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <string>

class Argument
{
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
   
   //bool  setDefault(int defaultVal = 0);
//   bool  setDefault(std::string &defaultVal = "");
//   bool  setDefault(float defaultVal = 0.0);
   
   void        name(const std::string &val){ m_sName = val; };
   std::string name(){ return m_sName; };
   
   void        primary(const std::string &val){ m_sId = val; };
   std::string primary(){ return m_sId; };
   
   void        alt(const std::string &val){ m_sAltId = val; };
   std::string alt(){ return m_sAltId; };
   
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
   
   void  initialize();
   
   bool  parse(const std::string &definition);
   
   std::string generate();
   
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

#endif // ARGUMENT_H