#ifndef STRUCTURE_H
#define STRUCTURE_H

#include <map>
#include <vector>
#include <string>
#include <sstream>
#include "BaseTypes.h"

//------------------------------------------------------------------------------
// Class Field
//------------------------------------------------------------------------------
class Field
{
public:
   
   enum Type
   {
      Null,
      Int32,
      Int64,
      Double,
      Bool,
      String,
      Block,
      Struct,
      Int32List,
      Int64List,
      DoubleList,
      BoolList,
      StringList,
      BlockList,
      StructList,
      Invalid
   };
   
   Field();
   Field(const Field &other);
   Field(Type type, const std::string &name);
   virtual ~Field();
   
   Type getType() const;
   
   const std::string& getName() const;
   
   virtual std::string toString() const = 0;
   
   static std::string TypeToString(Type type);
   
   Field& operator = (const Field& other);
   
private:
   
   Type mType;
   std::string mName;
};


//------------------------------------------------------------------------------
// Class TField
//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
class TField : public Field
{
public:
   
   TField();
   TField(const TField &other);
   TField(const std::string &name, const T &value, Field::Type type = TYPE);
   virtual ~TField();
   
   const T& getValue() const;
   T& getValue();
   
   std::string toString() const;
   
   TField& operator = (const TField &other);
   
protected:
   
   T mValue;
};


//------------------------------------------------------------------------------
// Class Structure
//------------------------------------------------------------------------------
class Structure
{
public:
   
   Structure(const std::string &name = "");
   Structure(const Structure &other);
   virtual ~Structure();
   
   void set(const std::string &name, i32 value); 
   bool get(const std::string &name, i32 &value) const;
   
   void set(const std::string &name, i64 value); 
   bool get(const std::string &name, i64 &value) const;
   
   void set(const std::string &name, double value); 
   bool get(const std::string &name, double &value) const;
   
   void set(const std::string &name, bool value); 
   bool get(const std::string &name, bool &value) const;
   
   void set(const std::string &name, const std::string &value); 
   bool get(const std::string &name, std::string &value) const;
   
   void set(const std::string &name, const Structure &value); 
   bool get(const std::string &name, Structure &value) const;
   
   void set(const std::string &name, const std::vector<i32> &value); 
   bool get(const std::string &name, std::vector<i32> &value) const;
   
   void set(const std::string &name, const std::vector<i64> &value); 
   bool get(const std::string &name, std::vector<i64> &value) const;
   
   void set(const std::string &name, const std::vector<bool> &value); 
   bool get(const std::string &name, std::vector<bool> &value) const;
   
   void set(const std::string &name, const std::vector<double> &value); 
   bool get(const std::string &name, std::vector<double> &value) const;
   
   void set(const std::string &name, const std::vector<std::string> &value); 
   bool get(const std::string &name, std::vector<std::string> &value) const;
   
   void set(const std::string &name, const std::vector<Structure> &value); 
   bool get(const std::string &name, std::vector<Structure> &value) const;
         
   bool hasField(const std::string &name) const;
   bool hasFieldOfType(const std::string &name, Field::Type type) const;
   Field::Type getFieldType(const std::string &name) const;
   
   void printFormatted() const;
   
   std::string toString() const;
   
   void getFieldList(std::vector<const Field*>& fields) const;
   
   std::string getName() const;
   
   void empty();
   
   Structure& operator = (const Structure &other);
   
private:

   void insertField(Field* pField);
   void deleteIfExists(const std::string &name);
         
private:
   
   std::map<std::string, Field*> mFieldMap;
   std::vector<Field*> mFieldList;
   
   std::string mName;
};

class TransportObject
{
public:
   
   enum Encoding {
      Text,
      Base64,
      UnknownEncoding
   };
   
   enum Format {
      JSON,
      XML,  // Not supported yet
      UnknownFormat
   };
   
   TransportObject(Format format) { mFormat = format; };
   /*TransportObject(Format format, const Structure &structure)
   {
      mFormat = format;
      from(structure);
   };*/
   virtual ~TransportObject(){};
   
   virtual void from(const Structure &structure) = 0;
   virtual void to(Structure &structure) const {};
   
   virtual bool getData(Encoding encoding, 
                        ui8** pData, 
                        ui32 &nSizeBytes) const = 0;
   
   virtual void setData(Encoding encoding,
                        const ui8* pData,
                        ui32 nSizeBytes) = 0;
      
   virtual std::string toString() const = 0;
   
   Format getFormat() const
   {
      return mFormat;
   }
   
protected:
   
   Format mFormat;
};



//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
TField<T, TYPE>::TField() : Field() {}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
TField<T, TYPE>::TField(const TField<T, TYPE> &other)
: Field(other) 
{
   *this = other;
}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
TField<T, TYPE>::TField(const std::string &name, 
                        const T& value, 
                        Field::Type type)
: Field(type, name),
  mValue(value)
{
}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
TField<T, TYPE>::~TField() {}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
T& TField<T, TYPE>::getValue()
{
   return mValue;
}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
const T& TField<T, TYPE>::getValue() const
{
   return mValue;
}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
std::string TField<T, TYPE>::toString() const
{
   std::stringstream lStream;
   /*
   lStream << "{";
   lStream << "name: " << getName() << ", ";
   lStream << "type: " << Field::TypeToString(getType()) << ", ";
   lStream << "value: " << getValue();
   lStream << "}";
   */
   return lStream.str();
}

//------------------------------------------------------------------------------
template<typename T, Field::Type TYPE>
TField<T, TYPE>& TField<T, TYPE>::operator = (const TField<T, TYPE> &other)
{
   if (this == &other)
   {
      return *this;
   }
   
   mValue = other.mValue;
   
   return *this;
}

#endif // STRUCTURE_H
