#include <stdio.h>
#include <fstream>
#include "Log.h"
#include "CircularBuffer.h"

using namespace coral;

int main()
{
  coral::log::level(coral::log::Debug);

  CircularBuffer buffer;

  buffer.allocate(256);
  if (buffer.capacity() != 256)
  {
    printf("capacity: exp=%u, act=%u\n", 256, buffer.capacity());
    return -1;
  }

  if (buffer.size() != 0)
  {
    printf("size: exp=%u, act=%u\n", 0, buffer.size());
    return -1;
  }

  if (buffer.isEmpty() != true)
  {
    printf("isEmpty: exp=%u, act=%u\n", true, buffer.isEmpty());
    return -1;
  }

  if (buffer.isFull() != false)
  {
    printf("isFull: exp=%u, act=%u\n", false, buffer.isFull());
    return -1;
  }


  std::ifstream ifs("server.file.dat");
  printf("Wrote %u bytes to buffer\n", buffer.write(ifs, 256));
  if (buffer.size() != 63)
  {
    printf("size: exp=%u, act=%u\n", 63, buffer.size());
  }
  ifs.close();

  char cbuff[256];
  memset(cbuff, 0, sizeof(cbuff));
  printf("Peeked %u bytes from buffer\n", buffer.peek(cbuff, 256));
  if (buffer.size() != 63)
  {
    printf("size: exp=%u, act=%u\n", 63, buffer.size());
  }

  printf("cbuff = %s\n", cbuff);

  memset(cbuff, 0, sizeof(cbuff));
  printf("Read %u bytes from buffer\n", buffer.read(cbuff, 256));
  if (buffer.size() != 0)
  {
    printf("size: exp=%u, act=%u\n", 0, buffer.size());
  }
  printf("cbuff = %s\n", cbuff);

  memset(cbuff, 0, sizeof(cbuff));
  printf("Peeked %u bytes from buffer\n", buffer.peek(cbuff, 256));
  if (buffer.size() != 63)
  {
    printf("size: exp=%u, act=%u\n", 63, buffer.size());
  }

  printf("cbuff = %s\n", cbuff);

  coral::log::flush();
  return 0;
}

