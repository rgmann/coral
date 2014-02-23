#include <unistd.h>
#include "Log.h"
#include "OtherLogUser.h"

int main (int argc, char* argv[])
{
  liber::log::options(liber::log::DisplayTimestamp | liber::log::DisplayLogLevel);
  liber::log::enable();

  liber::log::status("Program started: %s\n", argv[0]);
  liber::log::status("Program waiting %d seconds\n", 2);
  sleep(2);
  liber::log::error("Sleep done\n");

  OtherLogUser olu;
  olu.printAnInt(38);
  liber::log::status("Program done\n");

  sleep(2);
  return 0;
}


