#ifndef HASH_TYPES_H
#define HASH_TYPES_H

union Hash128 {
   unsigned char b[16];
   unsigned int  w[4];
};

#endif // HASH_TYPES_H