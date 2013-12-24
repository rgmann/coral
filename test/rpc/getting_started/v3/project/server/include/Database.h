#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <iostream>
#include "rpc_getting_started.pb.h"

using namespace rpc_getting_started;

class Database {
public:

   Database(){};
   ~Database(){};

   bool create (const std::string& firstname,
                const std::string& lastname,
                const UserAttributes& attrs)
   {
      bool lbSuccess = false;
      if (firstname.empty() || lastname.empty()) return false;
      if (mUsers.count(lastname) == 0)
      {
         lbSuccess  = true;
         std::vector<UserAttributes> attributes;
         mUsers.insert(std::make_pair(lastname, attributes));
         mUsers.find(lastname)->second.push_back(attrs);
      }
      else
      {
         UserAttributes user;
         std::vector<UserAttributes>& attributes = mUsers.find(lastname)->second;
         if (!findUser(firstname, lastname, user))
         {
            attributes.push_back(attrs);
            lbSuccess = true;
         }
      }

      return lbSuccess;
   };

   bool read (const std::string& firstname, 
              const std::string& lastname,
              std::vector<UserAttributes>& attributes)
   {
      bool lbSuccess = false;

      if (lastname.empty()) return false;
      if (mUsers.count(lastname) != 0)
      {
         if (firstname.empty())
         {
            attributes = mUsers.find(lastname)->second;
            lbSuccess = true;
         }
         else
         {
            UserAttributes user;
            if (findUser(firstname, lastname, user))
            {
               attributes.push_back(user);
               lbSuccess = true;
            }
         }
      }

      return lbSuccess;
   };

   bool update (const std::string& firstname,
                const std::string& lastname,
                const UserAttributes& attrs)
   {
      return create(firstname, lastname, attrs);
   };

   bool destroy (const std::string& firstname,
                 const std::string& lastname)
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

   bool findUser (const std::string& firstname,
                  const std::string& lastname,
                  UserAttributes& user)
   {
      bool lbExists = false;

      if (mUsers.count(lastname) != 0)
      {
         std::vector<UserAttributes>& attributes = mUsers.find(lastname)->second;
         std::vector<UserAttributes>::iterator lIt = attributes.begin();
         for (; lIt != attributes.end(); lIt++)
         {
            if (lIt->name().first() == firstname)
            {
               user.CopyFrom(*lIt);
               lbExists = true;
               break;
            }
         }
      }

      return lbExists;
   };

private:

   std::map<std::string, std::vector<UserAttributes> > mUsers;

};

#endif // DATABASE_H
