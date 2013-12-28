// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: rpc_getting_started.proto

#ifndef PROTOBUF_rpc_5fgetting_5fstarted_2eproto__INCLUDED
#define PROTOBUF_rpc_5fgetting_5fstarted_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace rpc_getting_started {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

class Parameters;
class Result;
class UserName;
class UserAttributes;
class UserAttributesList;
class DbStatus;

// ===================================================================

class Parameters : public ::google::protobuf::Message {
 public:
  Parameters();
  virtual ~Parameters();

  Parameters(const Parameters& from);

  inline Parameters& operator=(const Parameters& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Parameters& default_instance();

  void Swap(Parameters* other);

  // implements Message ----------------------------------------------

  Parameters* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Parameters& from);
  void MergeFrom(const Parameters& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated int32 values = 1;
  inline int values_size() const;
  inline void clear_values();
  static const int kValuesFieldNumber = 1;
  inline ::google::protobuf::int32 values(int index) const;
  inline void set_values(int index, ::google::protobuf::int32 value);
  inline void add_values(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      values() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_values();

  // @@protoc_insertion_point(class_scope:rpc_getting_started.Parameters)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > values_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static Parameters* default_instance_;
};
// -------------------------------------------------------------------

class Result : public ::google::protobuf::Message {
 public:
  Result();
  virtual ~Result();

  Result(const Result& from);

  inline Result& operator=(const Result& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Result& default_instance();

  void Swap(Result* other);

  // implements Message ----------------------------------------------

  Result* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Result& from);
  void MergeFrom(const Result& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional int32 op_count = 1;
  inline bool has_op_count() const;
  inline void clear_op_count();
  static const int kOpCountFieldNumber = 1;
  inline ::google::protobuf::int32 op_count() const;
  inline void set_op_count(::google::protobuf::int32 value);

  // optional int32 result = 2;
  inline bool has_result() const;
  inline void clear_result();
  static const int kResultFieldNumber = 2;
  inline ::google::protobuf::int32 result() const;
  inline void set_result(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:rpc_getting_started.Result)
 private:
  inline void set_has_op_count();
  inline void clear_has_op_count();
  inline void set_has_result();
  inline void clear_has_result();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 op_count_;
  ::google::protobuf::int32 result_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static Result* default_instance_;
};
// -------------------------------------------------------------------

class UserName : public ::google::protobuf::Message {
 public:
  UserName();
  virtual ~UserName();

  UserName(const UserName& from);

  inline UserName& operator=(const UserName& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const UserName& default_instance();

  void Swap(UserName* other);

  // implements Message ----------------------------------------------

  UserName* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserName& from);
  void MergeFrom(const UserName& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string first = 1;
  inline bool has_first() const;
  inline void clear_first();
  static const int kFirstFieldNumber = 1;
  inline const ::std::string& first() const;
  inline void set_first(const ::std::string& value);
  inline void set_first(const char* value);
  inline void set_first(const char* value, size_t size);
  inline ::std::string* mutable_first();
  inline ::std::string* release_first();
  inline void set_allocated_first(::std::string* first);

  // optional string last = 2;
  inline bool has_last() const;
  inline void clear_last();
  static const int kLastFieldNumber = 2;
  inline const ::std::string& last() const;
  inline void set_last(const ::std::string& value);
  inline void set_last(const char* value);
  inline void set_last(const char* value, size_t size);
  inline ::std::string* mutable_last();
  inline ::std::string* release_last();
  inline void set_allocated_last(::std::string* last);

  // @@protoc_insertion_point(class_scope:rpc_getting_started.UserName)
 private:
  inline void set_has_first();
  inline void clear_has_first();
  inline void set_has_last();
  inline void clear_has_last();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* first_;
  ::std::string* last_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static UserName* default_instance_;
};
// -------------------------------------------------------------------

class UserAttributes : public ::google::protobuf::Message {
 public:
  UserAttributes();
  virtual ~UserAttributes();

  UserAttributes(const UserAttributes& from);

  inline UserAttributes& operator=(const UserAttributes& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const UserAttributes& default_instance();

  void Swap(UserAttributes* other);

  // implements Message ----------------------------------------------

  UserAttributes* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserAttributes& from);
  void MergeFrom(const UserAttributes& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional .rpc_getting_started.UserName name = 1;
  inline bool has_name() const;
  inline void clear_name();
  static const int kNameFieldNumber = 1;
  inline const ::rpc_getting_started::UserName& name() const;
  inline ::rpc_getting_started::UserName* mutable_name();
  inline ::rpc_getting_started::UserName* release_name();
  inline void set_allocated_name(::rpc_getting_started::UserName* name);

  // optional sint32 age = 2;
  inline bool has_age() const;
  inline void clear_age();
  static const int kAgeFieldNumber = 2;
  inline ::google::protobuf::int32 age() const;
  inline void set_age(::google::protobuf::int32 value);

  // optional string eye_color = 3;
  inline bool has_eye_color() const;
  inline void clear_eye_color();
  static const int kEyeColorFieldNumber = 3;
  inline const ::std::string& eye_color() const;
  inline void set_eye_color(const ::std::string& value);
  inline void set_eye_color(const char* value);
  inline void set_eye_color(const char* value, size_t size);
  inline ::std::string* mutable_eye_color();
  inline ::std::string* release_eye_color();
  inline void set_allocated_eye_color(::std::string* eye_color);

  // @@protoc_insertion_point(class_scope:rpc_getting_started.UserAttributes)
 private:
  inline void set_has_name();
  inline void clear_has_name();
  inline void set_has_age();
  inline void clear_has_age();
  inline void set_has_eye_color();
  inline void clear_has_eye_color();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::rpc_getting_started::UserName* name_;
  ::std::string* eye_color_;
  ::google::protobuf::int32 age_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(3 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static UserAttributes* default_instance_;
};
// -------------------------------------------------------------------

class UserAttributesList : public ::google::protobuf::Message {
 public:
  UserAttributesList();
  virtual ~UserAttributesList();

  UserAttributesList(const UserAttributesList& from);

  inline UserAttributesList& operator=(const UserAttributesList& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const UserAttributesList& default_instance();

  void Swap(UserAttributesList* other);

  // implements Message ----------------------------------------------

  UserAttributesList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const UserAttributesList& from);
  void MergeFrom(const UserAttributesList& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .rpc_getting_started.UserAttributes users = 1;
  inline int users_size() const;
  inline void clear_users();
  static const int kUsersFieldNumber = 1;
  inline const ::rpc_getting_started::UserAttributes& users(int index) const;
  inline ::rpc_getting_started::UserAttributes* mutable_users(int index);
  inline ::rpc_getting_started::UserAttributes* add_users();
  inline const ::google::protobuf::RepeatedPtrField< ::rpc_getting_started::UserAttributes >&
      users() const;
  inline ::google::protobuf::RepeatedPtrField< ::rpc_getting_started::UserAttributes >*
      mutable_users();

  // @@protoc_insertion_point(class_scope:rpc_getting_started.UserAttributesList)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::rpc_getting_started::UserAttributes > users_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static UserAttributesList* default_instance_;
};
// -------------------------------------------------------------------

class DbStatus : public ::google::protobuf::Message {
 public:
  DbStatus();
  virtual ~DbStatus();

  DbStatus(const DbStatus& from);

  inline DbStatus& operator=(const DbStatus& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const DbStatus& default_instance();

  void Swap(DbStatus* other);

  // implements Message ----------------------------------------------

  DbStatus* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const DbStatus& from);
  void MergeFrom(const DbStatus& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional bool success = 1;
  inline bool has_success() const;
  inline void clear_success();
  static const int kSuccessFieldNumber = 1;
  inline bool success() const;
  inline void set_success(bool value);

  // @@protoc_insertion_point(class_scope:rpc_getting_started.DbStatus)
 private:
  inline void set_has_success();
  inline void clear_has_success();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  bool success_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_AssignDesc_rpc_5fgetting_5fstarted_2eproto();
  friend void protobuf_ShutdownFile_rpc_5fgetting_5fstarted_2eproto();

  void InitAsDefaultInstance();
  static DbStatus* default_instance_;
};
// ===================================================================


// ===================================================================

// Parameters

// repeated int32 values = 1;
inline int Parameters::values_size() const {
  return values_.size();
}
inline void Parameters::clear_values() {
  values_.Clear();
}
inline ::google::protobuf::int32 Parameters::values(int index) const {
  return values_.Get(index);
}
inline void Parameters::set_values(int index, ::google::protobuf::int32 value) {
  values_.Set(index, value);
}
inline void Parameters::add_values(::google::protobuf::int32 value) {
  values_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
Parameters::values() const {
  return values_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
Parameters::mutable_values() {
  return &values_;
}

// -------------------------------------------------------------------

// Result

// optional int32 op_count = 1;
inline bool Result::has_op_count() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Result::set_has_op_count() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Result::clear_has_op_count() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Result::clear_op_count() {
  op_count_ = 0;
  clear_has_op_count();
}
inline ::google::protobuf::int32 Result::op_count() const {
  return op_count_;
}
inline void Result::set_op_count(::google::protobuf::int32 value) {
  set_has_op_count();
  op_count_ = value;
}

// optional int32 result = 2;
inline bool Result::has_result() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Result::set_has_result() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Result::clear_has_result() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Result::clear_result() {
  result_ = 0;
  clear_has_result();
}
inline ::google::protobuf::int32 Result::result() const {
  return result_;
}
inline void Result::set_result(::google::protobuf::int32 value) {
  set_has_result();
  result_ = value;
}

// -------------------------------------------------------------------

// UserName

// optional string first = 1;
inline bool UserName::has_first() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UserName::set_has_first() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UserName::clear_has_first() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UserName::clear_first() {
  if (first_ != &::google::protobuf::internal::kEmptyString) {
    first_->clear();
  }
  clear_has_first();
}
inline const ::std::string& UserName::first() const {
  return *first_;
}
inline void UserName::set_first(const ::std::string& value) {
  set_has_first();
  if (first_ == &::google::protobuf::internal::kEmptyString) {
    first_ = new ::std::string;
  }
  first_->assign(value);
}
inline void UserName::set_first(const char* value) {
  set_has_first();
  if (first_ == &::google::protobuf::internal::kEmptyString) {
    first_ = new ::std::string;
  }
  first_->assign(value);
}
inline void UserName::set_first(const char* value, size_t size) {
  set_has_first();
  if (first_ == &::google::protobuf::internal::kEmptyString) {
    first_ = new ::std::string;
  }
  first_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserName::mutable_first() {
  set_has_first();
  if (first_ == &::google::protobuf::internal::kEmptyString) {
    first_ = new ::std::string;
  }
  return first_;
}
inline ::std::string* UserName::release_first() {
  clear_has_first();
  if (first_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = first_;
    first_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserName::set_allocated_first(::std::string* first) {
  if (first_ != &::google::protobuf::internal::kEmptyString) {
    delete first_;
  }
  if (first) {
    set_has_first();
    first_ = first;
  } else {
    clear_has_first();
    first_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// optional string last = 2;
inline bool UserName::has_last() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UserName::set_has_last() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UserName::clear_has_last() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UserName::clear_last() {
  if (last_ != &::google::protobuf::internal::kEmptyString) {
    last_->clear();
  }
  clear_has_last();
}
inline const ::std::string& UserName::last() const {
  return *last_;
}
inline void UserName::set_last(const ::std::string& value) {
  set_has_last();
  if (last_ == &::google::protobuf::internal::kEmptyString) {
    last_ = new ::std::string;
  }
  last_->assign(value);
}
inline void UserName::set_last(const char* value) {
  set_has_last();
  if (last_ == &::google::protobuf::internal::kEmptyString) {
    last_ = new ::std::string;
  }
  last_->assign(value);
}
inline void UserName::set_last(const char* value, size_t size) {
  set_has_last();
  if (last_ == &::google::protobuf::internal::kEmptyString) {
    last_ = new ::std::string;
  }
  last_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserName::mutable_last() {
  set_has_last();
  if (last_ == &::google::protobuf::internal::kEmptyString) {
    last_ = new ::std::string;
  }
  return last_;
}
inline ::std::string* UserName::release_last() {
  clear_has_last();
  if (last_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = last_;
    last_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserName::set_allocated_last(::std::string* last) {
  if (last_ != &::google::protobuf::internal::kEmptyString) {
    delete last_;
  }
  if (last) {
    set_has_last();
    last_ = last;
  } else {
    clear_has_last();
    last_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// UserAttributes

// optional .rpc_getting_started.UserName name = 1;
inline bool UserAttributes::has_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void UserAttributes::set_has_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void UserAttributes::clear_has_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void UserAttributes::clear_name() {
  if (name_ != NULL) name_->::rpc_getting_started::UserName::Clear();
  clear_has_name();
}
inline const ::rpc_getting_started::UserName& UserAttributes::name() const {
  return name_ != NULL ? *name_ : *default_instance_->name_;
}
inline ::rpc_getting_started::UserName* UserAttributes::mutable_name() {
  set_has_name();
  if (name_ == NULL) name_ = new ::rpc_getting_started::UserName;
  return name_;
}
inline ::rpc_getting_started::UserName* UserAttributes::release_name() {
  clear_has_name();
  ::rpc_getting_started::UserName* temp = name_;
  name_ = NULL;
  return temp;
}
inline void UserAttributes::set_allocated_name(::rpc_getting_started::UserName* name) {
  delete name_;
  name_ = name;
  if (name) {
    set_has_name();
  } else {
    clear_has_name();
  }
}

// optional sint32 age = 2;
inline bool UserAttributes::has_age() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void UserAttributes::set_has_age() {
  _has_bits_[0] |= 0x00000002u;
}
inline void UserAttributes::clear_has_age() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void UserAttributes::clear_age() {
  age_ = 0;
  clear_has_age();
}
inline ::google::protobuf::int32 UserAttributes::age() const {
  return age_;
}
inline void UserAttributes::set_age(::google::protobuf::int32 value) {
  set_has_age();
  age_ = value;
}

// optional string eye_color = 3;
inline bool UserAttributes::has_eye_color() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void UserAttributes::set_has_eye_color() {
  _has_bits_[0] |= 0x00000004u;
}
inline void UserAttributes::clear_has_eye_color() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void UserAttributes::clear_eye_color() {
  if (eye_color_ != &::google::protobuf::internal::kEmptyString) {
    eye_color_->clear();
  }
  clear_has_eye_color();
}
inline const ::std::string& UserAttributes::eye_color() const {
  return *eye_color_;
}
inline void UserAttributes::set_eye_color(const ::std::string& value) {
  set_has_eye_color();
  if (eye_color_ == &::google::protobuf::internal::kEmptyString) {
    eye_color_ = new ::std::string;
  }
  eye_color_->assign(value);
}
inline void UserAttributes::set_eye_color(const char* value) {
  set_has_eye_color();
  if (eye_color_ == &::google::protobuf::internal::kEmptyString) {
    eye_color_ = new ::std::string;
  }
  eye_color_->assign(value);
}
inline void UserAttributes::set_eye_color(const char* value, size_t size) {
  set_has_eye_color();
  if (eye_color_ == &::google::protobuf::internal::kEmptyString) {
    eye_color_ = new ::std::string;
  }
  eye_color_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* UserAttributes::mutable_eye_color() {
  set_has_eye_color();
  if (eye_color_ == &::google::protobuf::internal::kEmptyString) {
    eye_color_ = new ::std::string;
  }
  return eye_color_;
}
inline ::std::string* UserAttributes::release_eye_color() {
  clear_has_eye_color();
  if (eye_color_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = eye_color_;
    eye_color_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void UserAttributes::set_allocated_eye_color(::std::string* eye_color) {
  if (eye_color_ != &::google::protobuf::internal::kEmptyString) {
    delete eye_color_;
  }
  if (eye_color) {
    set_has_eye_color();
    eye_color_ = eye_color;
  } else {
    clear_has_eye_color();
    eye_color_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// -------------------------------------------------------------------

// UserAttributesList

// repeated .rpc_getting_started.UserAttributes users = 1;
inline int UserAttributesList::users_size() const {
  return users_.size();
}
inline void UserAttributesList::clear_users() {
  users_.Clear();
}
inline const ::rpc_getting_started::UserAttributes& UserAttributesList::users(int index) const {
  return users_.Get(index);
}
inline ::rpc_getting_started::UserAttributes* UserAttributesList::mutable_users(int index) {
  return users_.Mutable(index);
}
inline ::rpc_getting_started::UserAttributes* UserAttributesList::add_users() {
  return users_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::rpc_getting_started::UserAttributes >&
UserAttributesList::users() const {
  return users_;
}
inline ::google::protobuf::RepeatedPtrField< ::rpc_getting_started::UserAttributes >*
UserAttributesList::mutable_users() {
  return &users_;
}

// -------------------------------------------------------------------

// DbStatus

// optional bool success = 1;
inline bool DbStatus::has_success() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void DbStatus::set_has_success() {
  _has_bits_[0] |= 0x00000001u;
}
inline void DbStatus::clear_has_success() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void DbStatus::clear_success() {
  success_ = false;
  clear_has_success();
}
inline bool DbStatus::success() const {
  return success_;
}
inline void DbStatus::set_success(bool value) {
  set_has_success();
  success_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace rpc_getting_started

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_rpc_5fgetting_5fstarted_2eproto__INCLUDED
