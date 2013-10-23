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
   
   void set(i32 value); 
   bool get(i32 &value) const;
   
   void set(i64 value); 
   bool get(i64 &value) const;
   
   void set(double value); 
   bool get(double &value) const;
   
   void set(bool value); 
   bool get(bool &value) const;
   
   void set(const std::string &value); 
   bool get(std::string &value) const;
   
   void set(const Structure &value); 
   bool get(Structure &value) const;
   /*
   void set(const std::vector<i32> &value); 
   bool get(std::vector<i32> &value) const;
   
   void set(const std::vector<i64> &value); 
   bool get(std::vector<i64> &value) const;
   
   void set(const std::vector<bool> &value); 
   bool get(std::vector<bool> &value) const;
   
   void set(const std::vector<double> &value); 
   bool get(std::vector<double> &value) const;
   
   void set(const std::vector<std::string> &value); 
   bool get(std::vector<std::string> &value) const;
   
   void set(const std::vector<Structure> &value); 
   bool get(std::vector<Structure> &value) const;
   */
private:
   
   typedef Structure inherited;
};

#endif // RPC_RETURN_VALUE_H