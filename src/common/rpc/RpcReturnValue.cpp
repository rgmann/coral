#include "RpcReturnValue.h"

static const std::string RpcRetvalName("rpcretval");
static const std::string FieldName("rpcrv");

#define CONSTRUCT(type) \
RpcReturnValue::RpcReturnValue(type value) \
: Structure(RpcRetvalName) \
{ \
   inherited::set(FieldName, value); \
}

#define SET(type) \
void RpcReturnValue::setReturnValue(type value) \
{ \
   inherited::set(FieldName, value); \
}

#define GET(type) \
bool RpcReturnValue::getReturnValue(type &value) const \
{ \
   return inherited::get(FieldName, value); \
}

RpcReturnValue::RpcReturnValue() : Structure(RpcRetvalName) {}
CONSTRUCT(i32)
CONSTRUCT(i64)
CONSTRUCT(double)
CONSTRUCT(bool)
CONSTRUCT(const std::string&)
CONSTRUCT(const Structure&)

SET(i32)
GET(i32)

SET(i64)
GET(i64)

SET(double)
GET(double)

SET(bool)
GET(bool)

SET(const std::string&)
GET(std::string)

SET(const Structure&)
GET(Structure)
/*
SET(const std::vector<i32>&)
GET(std::vector<i32>)

SET(const std::vector<i64>&)
GET(std::vector<i64>)

SET(const std::vector<bool>&)
GET(std::vector<bool>)

SET(const std::vector<double>&)
GET(std::vector<double>)

SET(const std::vector<std::string>&)
GET(std::vector<std::string>)

SET(const std::vector<Structure>&)
GET(std::vector<Structure>)
*/
