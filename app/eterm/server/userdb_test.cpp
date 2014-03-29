#include <iostream>
#include <stdio.h>
#include "UserDb.h"

using namespace eterm;
using namespace rpc_eterm;

void print_user(User* pUser)
{
  if (pUser)
  {
    printf(" ID: %d\n", pUser->user_id());
    std::cout << " first: " << pUser->user_name().first_name() << std::endl;
    std::cout << " last: " << pUser->user_name().last_name() << std::endl;
    std::cout << " accesses: " << pUser->access_count() << std::endl;
  }
}

int main()
{
  UserDb db("data/userdb.bin");

  db.load();


  // Insert users.
  for (ui8 id = 0; id < 5; id++)
  {
    if (db.find(id) == NULL)
    {
      User* lpUser = new User();
      lpUser->set_user_id(id);
      lpUser->set_access_count(id + 1);
      switch (id)
      {
      case 0:
        lpUser->mutable_user_name()->set_first_name("Christoph");
        lpUser->mutable_user_name()->set_last_name("Waltz");
        break;
      case 1:
        lpUser->mutable_user_name()->set_first_name("Ralph");
        lpUser->mutable_user_name()->set_last_name("Fiennes");
        break;
      case 2:
        lpUser->mutable_user_name()->set_first_name("Judi");
        lpUser->mutable_user_name()->set_last_name("Dench");
        break;
      case 3:
        lpUser->mutable_user_name()->set_first_name("Rosamund");
        lpUser->mutable_user_name()->set_last_name("Pike");
        break;
      case 4:
        lpUser->mutable_user_name()->set_first_name("Michael");
        lpUser->mutable_user_name()->set_last_name("Fassbender");
        break;
      default: break;
      }

      if (db.create(lpUser))
      {
        //std::cout << "Created user #" << std::dec << id << std::endl;
        printf("Created user #%d\n", id);
      }
      else
      {
        printf("Failed to create user #%d.\n", id);
      }
    }
    else
    {
      printf("User #%d already exists:\n", id);
      print_user(db.find(id));
    }
  }

  db.store();

  return 0;
}

