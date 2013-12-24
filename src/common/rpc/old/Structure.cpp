#include <iostream>
#include "Structure.h"

#define FIELD(stype, etype, name) \
         static_cast<const TField<stype, etype>*>(mFieldMap.at(name))

#define VALUEOF(stype, etype, field) \
         static_cast<const TField<stype, etype>*>(field)->getValue()

//------------------------------------------------------------------------------
//                              Class Field
//------------------------------------------------------------------------------
Field::Field()
: mType(Field::Invalid),
  mName("")
{
}

//------------------------------------------------------------------------------
Field::Field(const Field &other)
{
   *this = other;
}

//------------------------------------------------------------------------------
Field::Field(Type type, const std::string &name)
: mType(type),
  mName(name)
{
}

//------------------------------------------------------------------------------
Field::~Field() {}

//------------------------------------------------------------------------------
Field::Type Field::getType() const
{
   return mType;
}

//------------------------------------------------------------------------------
const std::string& Field::getName() const
{
   return mName;
}

//------------------------------------------------------------------------------
std::string Field::TypeToString(Type type)
{
   std::string lTypeString = "";
   
   switch (type)
   {
      case Int32: lTypeString = "Int32"; break;
      case Int64: lTypeString = "Int64"; break;
      case Double: lTypeString = "Double"; break;
      case Bool: lTypeString = "Bool"; break;
      case String: lTypeString = "String"; break;
      case Block: lTypeString = "Block"; break;
      case Struct: lTypeString = "Struct"; break;
      default: lTypeString = "Invalid"; break;
   }
   
   return lTypeString;
}

//------------------------------------------------------------------------------
Field& Field::operator = (const Field &other)
{
   if (this == &other)
   {
      return *this;
   }
   
   mName = other.mName;
   mType = other.mType;
   
   return *this;
}

//------------------------------------------------------------------------------
//                            Class Structure
//------------------------------------------------------------------------------
#define STRUCT_SET(etype,stype) \
void Structure::set(const std::string &name, stype value) \
{ \
   deleteIfExists(name); \
   insertField(new TField<stype, etype>(name, value)); \
}
#define STRUCT_RSET(etype,stype) \
void Structure::set(const std::string &name, const stype &value) \
{ \
deleteIfExists(name); \
insertField(new TField<stype, etype>(name, value)); \
}
#define STRUCT_GET(etype,stype) \
bool Structure::get(const std::string &name, stype &value) const \
{ \
   if (!hasFieldOfType(name, etype)) return false; \
   value = FIELD(stype, etype, name)->getValue(); \
   return true; \
}

//------------------------------------------------------------------------------
Structure::Structure(const std::string &name)
: mName(name)
{
}

//------------------------------------------------------------------------------
Structure::Structure(const Structure &other)
{
   *this = other;
}

//------------------------------------------------------------------------------
Structure::~Structure()
{
   empty();
}

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, i32 value)
//{
//   deleteIfExists(name);
//   insertField(new TField<i32, Field::Int32>(name, value));
//}
STRUCT_SET(Field::Int32, i32)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, i32 &value) const
//{
//   if (!hasFieldOfType(name, Field::Int32)) return false;
//   value = FIELD(i64, Field::Int32, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::Int32, i32)

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, i64 value)
//{
//   deleteIfExists(name);
//   insertField(new TField<i64, Field::Int64>(name, value));
//}
STRUCT_SET(Field::Int64, i64)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, i64 &value) const
//{
//   if (!hasFieldOfType(name, Field::Int64)) return false;
//   value = FIELD(i64, Field::Int64, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::Int64, i64)

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, bool value)
//{
//   deleteIfExists(name);
//   insertField(new TField<bool, Field::Bool>(name, value));
//}
STRUCT_SET(Field::Bool, bool)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, bool &value) const
//{
//   if (!hasFieldOfType(name, Field::Bool)) return false;
//   value = FIELD(bool, Field::Bool, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::Bool, bool)

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, double value)
//{
//   deleteIfExists(name);
//   insertField(new TField<double, Field::Double>(name, value));
//}
STRUCT_SET(Field::Double, double)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, double &value) const
//{
//   if (!hasFieldOfType(name, Field::Double)) return false;
//   value = FIELD(double, Field::Double, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::Double, double)

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, const std::string &value)
//{
//   deleteIfExists(name);
//   insertField(new TField<std::string, Field::String>(name, value));
//}
STRUCT_RSET(Field::String, std::string)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, std::string &value) const
//{
//   if (!hasFieldOfType(name, Field::String)) return false;
//   value = FIELD(std::string, Field::String, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::String, std::string)

//------------------------------------------------------------------------------
//void Structure::set(const std::string &name, const Structure &value)
//{
//   deleteIfExists(name);
//   insertField(new TField<Structure, Field::Struct>(name, value));
//}
STRUCT_RSET(Field::Struct, Structure)

//------------------------------------------------------------------------------
//bool Structure::get(const std::string &name, Structure &value) const
//{
//   if (!hasFieldOfType(name, Field::Struct)) return false;
//   value = FIELD(Structure, Field::Struct, name)->getValue();
//   return true;
//}
STRUCT_GET(Field::Struct, Structure)

//------------------------------------------------------------------------------
bool Structure::hasField(const std::string &name) const
{
   return (mFieldMap.count(name) && mFieldMap.at(name));
}

//------------------------------------------------------------------------------
bool Structure::hasFieldOfType(const std::string &name, Field::Type type) const
{
   return (hasField(name) && (mFieldMap.at(name)->getType() == type));
}

//------------------------------------------------------------------------------
void Structure::getFieldList(std::vector<const Field*>& fields) const
{   
   std::vector<Field*>::const_iterator lIt = mFieldList.begin();
   for (; lIt != mFieldList.end(); lIt++)
   {
      fields.push_back(static_cast<const Field*> (*lIt));
   }   
}

//------------------------------------------------------------------------------
std::string Structure::getName() const
{
   return mName;
}

//------------------------------------------------------------------------------
void Structure::printFormatted() const
{
   std::cout << "{" << std::endl;
   std::map<std::string, Field*>::const_iterator lIt = mFieldMap.begin();
   for (; lIt != mFieldMap.end(); ++lIt)
   {
      if (lIt->second == NULL) continue;
      std::cout << "   " << lIt->second->toString() << "," << std::endl;
   }
   std::cout << "}" << std::endl;
}

//------------------------------------------------------------------------------
Field::Type Structure::getFieldType(const std::string &name) const
{
   return mFieldMap.at(name)->getType();
}

//------------------------------------------------------------------------------
void Structure::empty()
{
   std::vector<Field*>::iterator lField = mFieldList.begin();
   for (; lField != mFieldList.end(); lField++)
   {
      if (*lField)
      {
         delete *lField;
         *lField = NULL;
      }
   }
   mFieldList.clear();
   mFieldMap.clear();
}

//------------------------------------------------------------------------------
Structure& Structure::operator = (const Structure &other)
{
   if (this == &other)
   {
      return *this;
   }
   
   // Clear this instance's map and vector.
   empty();
   
   mName = other.mName;
   
   // Do we really want to duplicate all fields?
   std::vector<Field*>::const_iterator lField = other.mFieldList.begin();
   for (; lField != other.mFieldList.end(); lField++)
   {
      switch ((*lField)->getType())
      {
         case Field::Int32:
            set((*lField)->getName(), VALUEOF(i32, Field::Int32, *lField));
            break;
         case Field::Int64:
            set((*lField)->getName(), VALUEOF(i64, Field::Int64, *lField));
            break;
         case Field::Double:
            set((*lField)->getName(), VALUEOF(double, Field::Double, *lField));
            break;
         case Field::Bool:
            set((*lField)->getName(), VALUEOF(bool, Field::Bool, *lField));
            break;
         case Field::String:
            set((*lField)->getName(), VALUEOF(std::string, Field::String, *lField));
            break;
         //case Field::Block:
         //   set((*lField)->getName(), VALUEOF(ui8*, Field::Block, *lField));
         //   break;
         case Field::Struct:
            set((*lField)->getName(), VALUEOF(Structure, Field::Struct, *lField));
            break;
         default:
            break;
      }
   }
   
   return *this;
}

//------------------------------------------------------------------------------
void Structure::insertField(Field* pField)
{
   if (pField)
   {
      mFieldMap[pField->getName()] = pField;
      mFieldList.push_back(pField);
   }
}

//------------------------------------------------------------------------------
void Structure::deleteIfExists(const std::string &name)
{
   if (hasField(name))
   {
      std::vector<Field*>::iterator lField = mFieldList.begin();
      for (; lField != mFieldList.end(); lField++)
      {
         if ((*lField)->getName() == name)
         {
            delete *lField;
            *lField = NULL;
            mFieldList.erase(lField);
            break;
         }
      }
      //delete mFieldMap[name];
      mFieldMap.erase(name);
   }
}
