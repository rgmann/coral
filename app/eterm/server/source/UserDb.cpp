#include <fstream>
#include <iostream>
#include "PacketHelper.h"
#include "UserDb.h"


using namespace liber::netapp;
using namespace eterm;
using namespace rpc_eterm;
/*
//-----------------------------------------------------------------------------
std::string User::serialize()
{
  PacketCtor ctor;
  ctor.write(ID);
  ctor.writeCString(firstName);
  ctor.writeCString(lastName);
  ctor.write(accessCount);
  ctor.write((char*)&lastAccess, sizeof(struct timespec));
  return ctor.stream.str();
}

//-----------------------------------------------------------------------------
bool User::deserialize(const char* pData, ui32 nBytes)
{
  bool lbSuccess = true;

  PacketDtor dtor;
  dtor.stream.write(pData, nBytes);

  lbSuccess &= dtor.read(ID);
  lbSuccess &= (dtor.read(firstName) == PacketDtor::ReadOk);
  lbSuccess &= (dtor.read(lastName) == PacketDtor::ReadOk);
  lbSuccess &= dtor.read(accessCount);
  lbSuccess &= (dtor.read((char*)&lastAccess,
                          sizeof(struct timespec)) == PacketDtor::ReadOk);
  return lbSuccess;
}
*/
//-----------------------------------------------------------------------------
UserDb::UserDb(const std::string& dbFile)
: mDbFile(dbFile)
{
}

//-----------------------------------------------------------------------------
UserDb::~UserDb()
{
}

//-----------------------------------------------------------------------------
bool UserDb::load()
{
  if (mDbFile.length() == 0) return false;

  std::ifstream ifs(mDbFile, std::ios::binary);
  bool lbSuccess = ifs.is_open();

  if (lbSuccess)
  {
    while (ifs)
    {
      // User definitions are delimited by a 32-bit integer specifying the
      // length of the following packet.
      ui32 lnBytesToRead = 0;
      ifs.read((char*)&lnBytesToRead, sizeof(lnBytesToRead));
      if (ifs.fail())
      {
        std::cout << "UserDb::load: Failed to read packet size." << std::endl;
        break;
      }

      char* lpData = new char[lnBytesToRead];
      ifs.read(lpData, lnBytesToRead);

      std::string lUserString;
      lUserString.assign(lpData, lnBytesToRead);

      User* lpUser = new User();
      if (lpUser->ParseFromString(lUserString))
      {
        mUsers.push_back(lpUser);
        mMapper.insert(std::pair<ui8, User*>(lpUser->user_id(), lpUser));
      }
      else
      {
        std::cout << "UserDb::load: Failed to read packet from stream." << std::endl;
        delete lpUser;
      }

      delete[] lpData;
    }

    ifs.close();
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool UserDb::store()
{
  PacketCtor ctor;

  if (mDbFile.length() == 0) return false;

  std::ofstream ofs(mDbFile, std::ofstream::binary);
  bool lbSuccess = ofs.is_open();

  if (lbSuccess)
  {
    std::vector<User*>::iterator user = mUsers.begin();
    for (; user != mUsers.end(); ++user)
    {
      if (*user)
      {
        ui32 lnBytes = (*user)->ByteSize();
        ofs.write((char*)&lnBytes, sizeof(lnBytes));
        if (!(*user)->SerializeToOstream(&ofs))
        {
          std::cout << "UserDb::store: Failed to serialize packet to stream." << std::endl;
        }
      }
    }

    ofs.close();
  }

  return lbSuccess;
}

//-----------------------------------------------------------------------------
bool UserDb::create(User* pUser)
{
  bool lbCanCreate = pUser && (mMapper.count(pUser->user_id()) == 0);
  if (lbCanCreate)
  {
    mUsers.push_back(pUser);
    mMapper.insert(std::pair<ui8, User*>(pUser->user_id(), pUser));
  }
  return lbCanCreate;
}

//-----------------------------------------------------------------------------
User* UserDb::find(ui8 id)
{
  User* lpUser = NULL;
  if (mMapper.count(id) > 0)
  {
    lpUser = mMapper.find(id)->second;
  }
  return lpUser;
}

//-----------------------------------------------------------------------------
User* UserDb::destroy(ui8 id)
{
  User* lpUser = NULL;
  if (mMapper.count(id) > 0)
  {
    lpUser = mMapper.find(id)->second;
    mMapper.find(id)->second = NULL;
    mMapper.erase(id);

    std::vector<User*>::iterator user = mUsers.begin();
    for (; user != mUsers.end(); ++user)
    {
      if (lpUser == *user)
      {
        *user = NULL;
        break;
      }
    }
  }
  return lpUser;
}

//-----------------------------------------------------------------------------
void UserDb::destroyAll()
{
  std::vector<User*>::iterator user = mUsers.begin();
  for (; user != mUsers.end(); ++user)
  {
    if (*user)
    {
      if (mMapper.count((*user)->user_id()) > 0)
      {
        mMapper.erase((*user)->user_id());
      }

      delete *user;
      *user = NULL;
    }
  }

  mUsers.clear();
}

//-----------------------------------------------------------------------------
void UserDb::all(UserList& list)
{
  std::vector<User*>::iterator user = mUsers.begin();
  for (; user != mUsers.end(); ++user)
  {
    if (*user)
    {
      list.add_users()->CopyFrom(**user);
    }
  }
}

