
struct test_struct {

  struct{
    int value;
  } m;


  float stuff;
  struct inner_struct{
    struct inner_inner_inst {
      char inner_val;
    };
    inner_inner_inst val;
  };
  inner_struct def;
};

class TestClass {
public:
};

