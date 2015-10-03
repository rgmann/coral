#include <unistd.h>
#include <iostream>
#include "Timestamp.h"

int main(int argc, char* argv[])
{
  Timestamp ts = Timestamp::Now();

  std::cout << "ts = " << std::fixed << ts.fseconds() << std::endl;
  std::cout << "Now().fseconds() = " << std::fixed << Timestamp::Now().fseconds() << std::endl;

  Timestamp start = Timestamp::Now();
  sleep(1);
  std::cout << "delta = " << std::fixed << (Timestamp::Now() - start).fseconds() << std::endl;
  std::cout << "delta = " << std::fixed << (start - Timestamp::Now()).fseconds() << std::endl;

  Timestamp total;
  for (int count = 0; count < 8; count++)
  {
    start = Timestamp::Now();
    sleep(1);
    total += (Timestamp::Now() - start);
  }
  std::cout << "total = " << std::fixed << total.fseconds() << std::endl;

  total -= total;
  std::cout << "total = " << std::fixed << total.fseconds() << std::endl;

  total.sample();
  usleep(1154);
  total = Timestamp::Now() - total;
  std::cout << "total = " << std::fixed << total.fseconds() << std::endl;

  return 0;
}

