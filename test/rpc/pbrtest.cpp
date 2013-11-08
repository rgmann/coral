#include <iostream>
#include "Thread.h"
#include "BinarySem.h"
#include "RpcClient.h"
#include "RpcServer.h"
#include "CalculatorClientStub.h"
#include "CalculatorServerStub.h"


class BaseObject {
public:

   BaseObject(){};
   BaseObject(const BaseObject& obj) {
      *this = obj;
   };

   bool set(const std::string& name, int value) {
      bool lbSuccess = false;
      if (lbSuccess = (myMap.count(name) == 0)) {
         myMap.insert(std::make_pair(name, value));
      }
      return lbSuccess;
   };

   bool get(const std::string& name, int& value) {
      bool lbSucces = false;
      if (lbSuccess = (myMap.count(name) != 0)) {
         value = myMap.find(name)->second;
      }
      return lbSuccess;
   };

   bool hasField(const std::string& name) const {
      return (myMap.count(name) != 0);
   }

   BaseObject& operator = (const BaseObject& obj) {
      if (this == &obj) return *this;

      std::map<std::string, int>::const_iterator it = obj.myMap.begin();
      myMap.empty();
      for (; it < obj.myMap.end(); ++it) {
         myMap.insert(it);
      }

      return *this.
   };

private:

   std::map<std::string, int> myMap;
};

class ObjectA : public BaseObject {
public:

   ObjectA(int a, int b, int c) {
      BaseObject::set("a", a);
      BaseObject::set("b", b);
      BaseObject::set("c", c);
   };

   bool get(int& a, int& b, int& c) {
      bool lbSuccess = true;
      lbSuccess &= get("a", a);
      lbSuccess &= get("b", b);
      lbSuccess &= get("c", c);
      return lbSuccess;
   };
};



int main()
{
   
   return 0;
}


