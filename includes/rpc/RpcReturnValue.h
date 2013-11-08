#ifndef RPC_RETURN_VALUE_H
#define RPC_RETURN_VALUE_H

#include "Structure.h"

class RpcReturnValue : public Structure
{
public:
   
   RpcReturnValue();
   RpcReturnValue(i32 value);
   RpcReturnValue(i64 value);
   RpcReturnValue(double value);
   RpcReturnValue(bool value);
   RpcReturnValue(const std::string& value);
   RpcReturnValue(const Structure& value);
   
   void setReturnValue(i32 value); 
   bool getReturnValue(i32 &value) const;
   
   void setReturnValue(i64 value); 
   bool getReturnValue(i64 &value) const;
   
   void setReturnValue(double value); 
   bool getReturnValue(double &value) const;
   
   void setReturnValue(bool value); 
   bool getReturnValue(bool &value) const;
   
   void setReturnValue(const std::string &value); 
   bool getReturnValue(std::string &value) const;
   
   void setReturnValue(const Structure &value); 
   bool getReturnValue(Structure &value) const;
   /*
   void setReturnValue(const std::vector<i32> &value); 
   bool getReturnValue(std::vector<i32> &value) const;
   
   void setReturnValue(const std::vector<i64> &value); 
   bool getReturnValue(std::vector<i64> &value) const;
   
   void setReturnValue(const std::vector<bool> &value); 
   bool getReturnValue(std::vector<bool> &value) const;
   
   void setReturnValue(const std::vector<double> &value); 
   bool getReturnValue(std::vector<double> &value) const;
   
   void setReturnValue(const std::vector<std::string> &value); 
   bool getReturnValue(std::vector<std::string> &value) const;
   
   void setReturnValue(const std::vector<Structure> &value); 
   bool getReturnValue(std::vector<Structure> &value) const;
   */
private:
   
   typedef Structure inherited;
};

#endif // RPC_RETURN_VALUE_H
