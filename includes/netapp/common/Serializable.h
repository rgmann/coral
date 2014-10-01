#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include <string>
#include <iostream>
#include <sstream>
#include "BaseTypes.h"

namespace liber {
namespace netapp {

enum ByteOrder {
   HostByteOrder = 0,
   NetworkByteOrder
};

void swapInPlace(i8& val);
void swapInPlace(ui8& val);
void swapInPlace(i16& val);
void swapInPlace(ui16& val);
void swapInPlace(i32& val);
void swapInPlace(ui32& val);
void swapInPlace(i64& val);
void swapInPlace(ui64& val);

class SerialStream {
public:

  SerialStream(ByteOrder endianness = HostByteOrder);

  void assign(const std::string&);
  void assign(const void* data, ui32 nBytes);

  /**
   * Serialization Methods
   */
  void write(ui8 val);
  void write(ui16 val);
  void write(ui32 val);
  void write(ui64 val);
  //  Appends a null character.
  void writeCString(const std::string& val);
  void write(const std::string& val);
  void write(const char* pData, ui32 nBytes);
  void write(bool val);

  /**
   * Deserialization Methods
   */
  enum ReadStatus {
    ReadOk,
    ReadFail,
    ReadEmpty
  };

  bool read(ui8& val);
  bool read(i8& val);
  bool read(ui16& val);
  bool read(i16& val);
  bool read(ui32& val);
  bool read(i32& val);
  bool read(ui64& val);
  bool read(i64& val);
  bool read(bool& val);

  // Assumes a null termination character.
  ReadStatus readCString(std::string& val);

  // Does not assume a null terminating character.
  ReadStatus read(std::string& val);
  ReadStatus read(char** ppData, ui32& nBytes);
  ReadStatus read(char* pData, ui32 nMaxBytes);

  std::stringstream stream;

private:

  ByteOrder mByteOrder;
};


class Serializable {
public:

  Serializable() {};
  virtual ~Serializable() {};

  std::string serialize() const;
  void serialize(SerialStream& ctor) const;

  std::string serialize();
  void serialize(SerialStream& ctor);

  bool deserialize(const char* pData, ui32 nSizeBytes);
  bool deserialize(const std::string& data);
  bool deserialize(SerialStream& dtor);

protected:

  virtual void pack(SerialStream& rCtor) {};
  virtual void pack(SerialStream& rCtor) const = 0;
  virtual bool unpack(SerialStream& rDtor) = 0;

};

} // End namespace netapp
} // End namespace liber

#endif // SERIALIZABLE_H

