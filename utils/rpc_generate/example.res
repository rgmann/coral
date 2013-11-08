#resource Calculator {
#
#  allow_exceptions = true;
#
#  i32 add(i32 a, i32 b);
#  i32 sub(i32 a, i32 b);
#  i32 mul(i32 a, i32 b);
#  i32 div(i32 a, i32 b);
#  i32 getNumOps();
#}

structure UserAttributes {
  string first_name;
  i32    age;
  string eye_color;
}

resource Database {

#  allow_exceptions = true;
  bool create(string lastname, cref UserAttributes attrs);
  bool read(string lastname, ref UserAttributes attrs);
  bool update(string lastname, cref UserAttributes attrs);
  bool destroy(string lastname);
}

