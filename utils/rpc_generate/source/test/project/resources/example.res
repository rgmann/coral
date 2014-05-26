
import_proto "other.proto";
import_proto 'base.proto';
namespace calculator_app;

resource Calculator {

  allow_exceptions = true;

  i32 add(i32 a, i32 b);
  i32 sub(i32 a, i32 b);
  i32 mul(i32 a, i32 b);
  i32 div(i32 a, i32 b);
  i32 getNumOps();
}

message UserAttributes {
  optional string first_name = 1;
  optional sint32 age        = 2;
  optional string eye_color  = 3;
}

resource Database {
  allow_exceptions = true;

  bool create(string lastname, cref UserAttributes attrs);
  bool read(string lastname, ref UserAttributes attrs);
  bool update(string lastname, cref UserAttributes attrs);
  bool destroy(string lastname);
}

