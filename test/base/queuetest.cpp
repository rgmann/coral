#include <stdio.h>
#include <unistd.h>
#include "Thread.h"
#include "Queue.h"

void  threadFunc(ThreadArg* pArg);

int main(int argc, char *argv[])
{
   char l_cChar = 0;
   Thread*  locpThread = NULL;
   Queue<char> charQ;
      
   printf("Queue test\n");
   
   charQ.initialize(2);
   
   locpThread = Thread::Create(threadFunc, (void*)&charQ);
   
   if (locpThread == NULL)
   {
      printf("Failed to create thread!\r\n");
      return 1;
   }
   
   
   while ((l_cChar = getchar()) != 'Q')
   {
      if (!charQ.push(l_cChar, 500))
      {
         printf("Timedout adding character to queue\n");
      }
   }
   
   locpThread->stop();
   locpThread->join();
      
   delete locpThread;
   locpThread = NULL;
   
   return 0;
}

void  threadFunc(ThreadArg* pArg)
{
   char l_cChar = 0;
   Queue<char>* l_pCharQ = NULL;
   
   l_pCharQ = reinterpret_cast<Queue<char>*>(pArg->pUserData);
   
   printf("Char thread started!\n");
   
   if (l_pCharQ == NULL)
   {
      printf("Queue pointer is NULL\n");
      return;
   }
   
   while (!pArg->stopSignalled())
   {
      if (l_pCharQ->pop(l_cChar, 500))
      {
         putchar(l_cChar);
      }
   }
   
   printf("Thread done!\n");
}
