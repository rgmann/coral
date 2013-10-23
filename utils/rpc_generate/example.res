resource Calculator {
  i32 add(i32 a, i32 b),
  i32 sub(i32 a, i32 b),
  i32 mul(i32 a, i32 b),
  i32 div(i32 a, i32 b),
  i32 getNumOps()
}

structure UserAttributes {
  string first_name,
  i32    age,
  string eye_color
}

resource Database {
  bool create(string lastname, UserAttributes attrs),
  bool read(string lastname, UserAttributes& attrs),
  bool update(string lastname, UserAttributes attrs),
  bool delete(string lastname)
}

