#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <string>
#include <utility>
#include "UserAttributes.h"

class Database {
public:

   Database(){};
   ~Database(){};

   bool create(std::string lastname, const UserAttributes& attrs)
   {
      bool lbSuccess = false;
      if (mUsers.count(lastname) == 0)
      {
         lbSuccess  = true;
         mUsers.insert(std::make_pair(lastname, attrs));
      }
      return lbSuccess;
   };

   bool read(const std::string& lastname, UserAttributes& attrs)
   {
      bool lbSuccess = false;
      if (mUsers.count(lastname) != 0)
      {
         lbSuccess = true;
         attrs = mUsers.find(lastname)->second;
      }
      return lbSuccess;
   };

   bool update(const std::string& lastname, const UserAttributes& attrs)
   {
      return create(lastname, attrs);
   };

   bool destroy(const std::string& lastname)
   {
      bool lbSuccess = false;
      if (mUsers.count(lastname) != 0)
      {
         lbSuccess = true;
         mUsers.erase(lastname);
      }
      return lbSuccess;
   };

private:

   std::map<std::string, UserAttributes> mUsers;

};

#endif // DATABASE_H
