#include "Log.h"
#include "FileSystemInterface.h"

int main()
{
  liber::log::level(liber::log::Debug);
  liber::rsync::FileSystemInterface fsif;

  fsif.setRoot("test_root");
  if (fsif.exists("test_file.txt"))
  {
    liber::log::status("test_file.txt exists.\n");
  }
  else
  {
    liber::log::status("test_file.txt does not exist.\n");
  }

  if (fsif.exists("file_a.txt") && fsif.exists("file_b.txt"))
  {
    fsif.swap("file_a.txt", "file_b.txt");
  }

  liber::log::flush();
  return 0;
}

