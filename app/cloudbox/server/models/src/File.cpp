#include "File.h"
#include "User.h"
#include "Group.h"
#include "Lock.h"
#include "FilePath.h"

#define  TYPE_UNKNOWN_STRING  ((const char*)"unknown")
#define  TYPE_FILE_STRING  ((const char*)"file")
#define  TYPE_DIR_STRING  ((const char*)"directory")
#define  TYPE_LINK_STRING  ((const char*)"link")

//------------------------------------------------------------------------------
File::File(MongoConnection *pConnection)
: GenericModel("files", pConnection)
{
   setDefaults();
}

//------------------------------------------------------------------------------
void File::setDefaults()
{
   ReadLock l_readLock;
   WriteLock l_writeLock;
   
   m_bsonObject = BSON(FILE_TYPE << "" <<
                       FILE_SIZE << 0 <<
                       FILE_READ_LOCK << l_readLock.toObj() <<
                       FILE_WRITE_LOCK << l_writeLock.toObj() <<
                       FILE_OWNER << mongo::jstNULL <<
                       FILE_GROUPS << mongo::jstNULL << 
                       FILE_PATH << BSON("name" << "" <<
                                         "path" << "" <<
                                         "extension" << ""));
}

//------------------------------------------------------------------------------
void File::type(FileType type)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append(FILE_TYPE, typeToString(type));
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
FileType File::type()
{   
   return stringToType(std::string(m_bsonObject.getStringField(FILE_TYPE)));
}

//------------------------------------------------------------------------------
std::string File::typeToString(FileType type)
{
   switch (type)
   {
      case File:        return std::string(TYPE_FILE_STRING);
      case Directory:   return std::string(TYPE_DIR_STRING);
      case Link:        return std::string(TYPE_LINK_STRING);
      default:          return std::string(TYPE_UNKNOWN_STRING);
   }
}

//------------------------------------------------------------------------------
FileType File::stringToType(const std::string &type)
{
   std::string l_sType(type);
   FileType    l_fileType = Unknown;
   
   if (l_sType == std::string(TYPE_FILE_STRING))
   {
      l_fileType = File;
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
void File::path(const FilePath &filePath)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("filepath", BSON("path" << filePath.path() <<
                                        "file" << filePath.file()));
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
FilePath File::path()
{
   mongo::BSONObj pathObj;
   
   pathObj = m_bsonObject.getObjectField("filepath");
   
   return FilePath(pathObj.getStringField("path"),
                         pathObj.getStringField("file"));
}

//------------------------------------------------------------------------------
std::string File::ext(const std::string &ext)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("ext", ext);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::string File::ext()
{
   return std::string(m_bsonObject.getStringField("ext"));
}

//------------------------------------------------------------------------------
bool File::group(Group &group)
{
   bool l_bSuccess = false;
   
   if (m_pDbConn->isConnected())
   {
      mongo::BSONObj grpIdElement;
      
      grpIdElement = m_bsonObject["owner"];
      
      l_bSuccess = findOne(BSON("_id" << grpIdElement), group);
   }
   
   return l_bSuccess;
}

//------------------------------------------------------------------------------
bool File::isLocked()
{
   bool l_bIsLocked = false;
   
   mongo::BSONElement lockObj;
      
   lockObj = m_bsonObject["lock"];
      
   // File is locked if the lock is not NULL.
   l_bIsLocked = (lockObj.type() != mongo::jstNULL);
   
   return l_bIsLocked;
}
