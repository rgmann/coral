// 
// Copyright (c) 2015, Robert Glissmann
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
// 
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 

// %% license-end-token %%
// 
// Author: Robert.Glissmann@gmail.com (Robert Glissmann)
// 
// 

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

