#include "File.h"
#include "User.h"
#include "Group.h"
#include "FilePath.h"

#define  TYPE_UNKNOWN_STRING  ((const char*)"unknown")
#define  TYPE_FILE_STRING  ((const char*)"file")
#define  TYPE_DIR_STRING  ((const char*)"directory")
#define  TYPE_LINK_STRING  ((const char*)"link")

#define FILE_TYPE ((const char*)"type")
#define FILE_SIZE ((const char*)"size")
#define FILE_OWNER ((const char*)"owner")
#define FILE_GROUPS ((const char*)"groups")
#define FILE_PATH ((const char*)"path")

//------------------------------------------------------------------------------
File::File()
: GenericModel("files")
{
   setDefaults();
}

//------------------------------------------------------------------------------
void File::setDefaults()
{
   m_bsonObject = BSON(FILE_TYPE << "" <<
                       FILE_SIZE << 0 <<
                       //FILE_READ_LOCK << l_readLock.toObj() <<
                       //FILE_WRITE_LOCK << l_writeLock.toObj() <<
                       FILE_OWNER << mongo::jstNULL <<
                       FILE_GROUPS << mongo::jstNULL << 
                       FILE_PATH << mongo::jstNULL);
}

//------------------------------------------------------------------------------
void File::type(Type type)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append(FILE_TYPE, TypeToString(type));
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
File::Type File::type() const
{   
   return StringToType(std::string(m_bsonObject.getStringField(FILE_TYPE)));
}

//------------------------------------------------------------------------------
std::string File::TypeToString(Type type)
{
   switch (type)
   {
      case Regular:        return std::string(TYPE_FILE_STRING);
      case Directory:   return std::string(TYPE_DIR_STRING);
      case Link:        return std::string(TYPE_LINK_STRING);
      default:          return std::string(TYPE_UNKNOWN_STRING);
   }
}

//------------------------------------------------------------------------------
File::Type File::StringToType(const std::string &type)
{
   std::string l_sType(type);
   Type    l_fileType = Unknown;
   
   if (l_sType == std::string(TYPE_FILE_STRING))
   {
      l_fileType = Regular;
   }
   else if (l_sType == std::string(TYPE_DIR_STRING))
   {
      l_fileType = Directory;
   }
   else if (l_sType == std::string(TYPE_LINK_STRING))
   {
      l_fileType = Link;
   }

   return l_fileType;
}

//------------------------------------------------------------------------------
void File::setPath(const FilePath &filePath)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("path", filePath.getFullPath());
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
FilePath File::getPath()
{
   return FilePath(m_bsonObject.getStringField("path"));
}

//------------------------------------------------------------------------------
std::string File::ext()
{
   FilePath lFilePath = getPath();
   return lFilePath.getExtension();
}

//------------------------------------------------------------------------------
//bool File::group(Group &group)
//{
//   bool l_bSuccess = false;
//   
//   if (m_pDbConn->isConnected())
//   {
//      mongo::BSONObj grpIdElement;
//      
//      grpIdElement = m_bsonObject["owner"];
//      
//      l_bSuccess = findOne(BSON("_id" << grpIdElement), group);
//   }
//   
//   return l_bSuccess;
//}

//------------------------------------------------------------------------------
bool File::isLocked() const
{
   bool l_bIsLocked = false;
   
   mongo::BSONElement lockObj;
      
   lockObj = m_bsonObject["lock"];
      
   // File is locked if the lock is not NULL.
   l_bIsLocked = (lockObj.type() != mongo::jstNULL);
   
   return l_bIsLocked;
}
