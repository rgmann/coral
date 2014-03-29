#ifndef USER_DB_H
#define USER_DB_H

#include <map>
#include <vector>
#include <string>
#include "BaseTypes.h"
#include "rpc_eterm.pb.h"

namespace eterm {

class UserDb {
public:

  UserDb(const std::string& dbFile);
  ~UserDb();

  bool load();
  bool store();

  bool  create(rpc_eterm::User* pUser);
  rpc_eterm::User* find(ui8 id);
  rpc_eterm::User* destroy(ui8 id);

  void destroyAll();
  void all(rpc_eterm::UserList& list);

private:

  std::string mDbFile;
  std::vector<rpc_eterm::User*> mUsers;
  std::map<ui8, rpc_eterm::User*> mMapper;
};

} // End namespace eterm

#endif // USER_DB_H
