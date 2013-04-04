#include "Thread.h"
#include "CountingSem.h"
#include <stdio.h>
#include <unistd.h>

void  threadFunc(ThreadArg* pArg);
CountingSem g_Sem(0);

int main(int argc, char *argv[])
{
   Thread*  locpThread = NULL;
      
   printf("Thread test\n");
   
   locpThread = Thread::Create(threadFunc, (void*)"Thread running");
   
   if (locpThread == NULL)
   {
      printf("Failed to create thread!\r\n");
      return 1;
   }
   
   sleep(2);
   //printf("Posting sem\n");
   //g_Sem.give();
   
   sleep(6);
   
   locpThread->stop();
   locpThread->join();
      
   delete locpThread;
   locpThread = NULL;
   
   return 0;
}

void  threadFunc(ThreadArg* pArg)
{
   //Sem::SemWaitForever
   g_Sem.take(5000);
   
   while (!pArg->stopSignalled())
   {
      printf("%s!\n", (char*)pArg->pUserData);
      sleep(1);
   }
   
   printf("Thread done!\r\n");
}
