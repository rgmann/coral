struct __attribute__ ((__packed__)) another_struct {
  union {
    uint8 bytes[30];
    struct {
      unsigned a : 2;
      unsigned b : 5;
    } bits;
  } data;
} another_struct_t;
