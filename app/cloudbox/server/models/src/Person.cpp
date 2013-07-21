#include "Person.h"

//------------------------------------------------------------------------------
Person::Person()
: GenericModel("person")
{
}

//------------------------------------------------------------------------------
Person::~Person()
{
}

//------------------------------------------------------------------------------
void Person::name(const std::string &name)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("name", name);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::string Person::name() const
{
   return std::string(m_bsonObject.getStringField("name"));
}

//------------------------------------------------------------------------------
void Person::age(int age)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("age", age);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
int Person::age() const
{
   return object().getIntField("age");
}

//------------------------------------------------------------------------------
void Person::city(const std::string &city)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("city", city);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::string Person::city() const
{
   return std::string(object().getStringField("city"));
}

//------------------------------------------------------------------------------
void Person::state(const std::string &state)
{
   mongo::BSONObjBuilder l_objBuilder;
   
   l_objBuilder.appendElementsUnique(m_bsonObject);
   l_objBuilder.append("state", state);
   
   m_bsonObject = l_objBuilder.obj();
}

//------------------------------------------------------------------------------
std::string Person::state() const
{
   return std::string(object().getStringField("state"));
}

//------------------------------------------------------------------------------
ui32 Person::visitCount() const
{
   return object().getIntField("visits");
}
