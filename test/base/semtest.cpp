#include "Thread.h"
#include "CountingSem.h"
#include <stdio.h>
#include <unistd.h>

void  threadFunc(ThreadArg* pArg);
CountingSem g_Sem(1);

int main(int argc, char *argv[])
{
   Thread*  locpThrA = NULL;
   Thread*  locpThrB = NULL;
      
   printf("Semaphore test\n");
   
   locpThrA = Thread::Create(threadFunc, (void*)"Thread A");
   
   if (locpThrA == NULL)
   {
      printf("Failed to create thread!\r\n");
      return 1;
   }
   
   locpThrB = Thread::Create(threadFunc, (void*)"Thread B");
   
   if (locpThrA == NULL)
   {
      printf("Failed to create thread!\r\n");
      return 1;
   }
   
   sleep(2);
   
   printf("Posting sem\n");
   g_Sem.give();
   
   sleep(6);
   
   locpThrA->stop();
   locpThrA->join();
   
   locpThrB->stop();
   locpThrB->join();
      
   delete locpThrA;
   locpThrA = NULL;
   
   delete locpThrB;
   locpThrB = NULL;
   
   return 0;
}

void  threadFunc(ThreadArg* pArg)
{
   printf("%s: Started!\n", (char*)pArg->pUserData);
   
   g_Sem.take(5000);
   
   printf("%s: Took sem!\n", (char*)pArg->pUserData);
   
   while (!pArg->stopSignalled())
   {
      sleep(1);
   }
   
   printf("%s: Thread done!\r\n", (char*)pArg->pUserData);
}
