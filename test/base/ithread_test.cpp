#include "IThread.h"
#include "CountingSem.h"
#include <stdio.h>
#include <unistd.h>

CountingSem g_Sem(0);

class TestThread : public IThread {
public:

  TestThread(CountingSem& rSem) : IThread("TestThread"), mrSem(rSem) {};

private:

  void run(const bool& bShutdown)
  {
    mrSem.take(5000);

    //while (!bShutdown)
    while (true)
    {
      printf("%s!\n", "Thread running");
      sleep(1);
    }
   
    printf("Thread done!\r\n");
  };

private:

  CountingSem& mrSem;
};

int main(int argc, char *argv[])
{
  CountingSem  countingSem(0);
  TestThread   thread(countingSem);
      
  printf("Thread test\n");

  for (int count = 0; count < 2; count++)
  {   
    thread.launch();
   
    sleep(2);
   
    sleep(6);
   
    //thread.join();
    thread.cancel();
    printf("Thread joined!\n\n");
  }

  return 0;
}

