#ifndef CRUD_ROUTER_H
#define CRUD_ROUTER_H

#include "CreateMsg.h"
#include "ReadMsg.h"
#include "UpdateMsg.h"
#include "DeleteMsg.h"
#include "Thread.h"
#include "Mutex.h"

class CrudRouter
{
public:
   
   enum Operation
   {
      Create = 0,
      Read,
      Update,
      Delete,
      NumOps
   }
   
   typedef void (*CrudCb)(Operation, void* pOwner, CrudMsg*);
   
   CrudRouter();
   
   ~CrudRouter();
   
   bool initialize(bool bThreaded);
   
   bool pushMsg(CrudMsg* pMsg);
   
   bool hook(Operation op, void* pOwner, CrudCb pFunc);
   
   bool unhook(Operation op, void* pOwner);
   
   void routeMessage(ui32 nTimeoutMs);
   
private:
   
   static void MsgProcThread(ThreadArg* pArg);
   
   void msgProcThread(ThreadArg* pArg);
   
private:
   
   struct CbNode
   {
      void*    pOwner;
      CrudCb   pFunc;
      CbNode*  pNextNode;
   };
   
   CbNode*  mOpHooks[NumOps];
   
   Queue<CrudMsg*> mCrudMsgQueue;
   
   Thread*  mpMsgProcThread;
   
   Mutex    mHookMutex;
};

#endif // CRUD_ROUTER_H