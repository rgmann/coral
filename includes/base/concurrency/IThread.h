#ifndef ITHREAD_H
#define ITHREAD_H

#include "Thread.h"

class IThread
{
public:
   
   IThread(const std::string &name = "");
   
   ~IThread();
   
   bool start();
   
   bool stop();
   
   virtual void run(bool &bShutdown) = 0;
   
   i32 getId() const;
   
   const std::string& getName() const;
   
private:
   
   static void ThreadFunc(ThreadArg* pArg);
   
private:
   
   Thread*     mpThread;
   
   std::string mName;
};

#endif // ITHREAD_H