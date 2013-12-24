
namespace getting_started;

message Parameters {
  repeated int32 values = 1;
}

message Result {
  optional int32 op_count = 1;
  optional int32 result   = 2;
}

resource Calculator {

  allow_exceptions = true;

  Result add (Parameters);
  Result sub (Parameters);
  Result mul (Parameters);
  Result div (Parameters);
  Result getNumOps (Parameters);
}

message UserName {
  optional string first = 1;
  optional string last  = 2;
}
message UserAttributes {
  optional UserName name      = 1;
  optional sint32   age       = 2;
  optional string   eye_color = 3;
}
message UserAttributesList {
  repeated UserAttributes users = 1;
}
message DbStatus {
  optional bool success = 1;
}

resource Database {
  allow_exceptions = true;

  DbStatus create(UserAttributes);
  UserAttributesList read(UserName);
  DbStatus update(UserAttributes);
  DbStatus destroy(UserName);
}

