
tab_size = 2;

enum {
  ITEM_ONE = 0,
  ITEM_TWO,
  ITEM_THREE
};

struct test_struct {

  exceptions: no;

  struct{
    int value;
  } m;


  float stuff;
  struct inner_struct{
    struct inner_inner_inst {
      char inner_val;
      struct {
        uint8 param;
      } deep;
    };
    inner_inner_inst val;
  };
  inner_struct def;

  struct __attribute__ ((packed)) {
    char a;
    char b;
  } d, r;

  struct empty {} emptydef;
};

class TestClass {
public:

  // Comment with a {
  //
  //
  // } in it.

  virtual int method(const test_struct& p1, bool p2)
  {
    // Do some stuff in here.
  }
};

resource Service {
  allow_exceptions = true;
  void method(int param1, float param2);
};


int main(int argc, char* argv[])
{
  return 0;
}
