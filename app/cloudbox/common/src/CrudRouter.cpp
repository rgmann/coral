#include "CrudRouter.h"

//------------------------------------------------------------------------------
CrudRouter::CrudRouter()
: mpMsgProcThread(NULL)
{
   for (ui32 lnOp = 0; lnOp < NumOps; lnOp++)
   {
      mOpHooks[lnOp] = NULL;
   }
}

//------------------------------------------------------------------------------
CrudRouter::~CrudRouter()
{
   if (mpMsgProcThread)
   {
      mpMsgProcThread->stop();
      mpMsgProcThread->join();
      
      delete mpMsgProcThread;
      mpMsgProcThread = NULL;
   }
   
   mHookMutex.lock();
   for (ui32 lnOp = 0; lnOp < NumOps; lnOp++)
   {
      while (mOpHooks[lnOp])
      {
         CbNode* lpTempNode = mOpHooks[lnOp]->pNextNode;
         delete mOpHooks[lnOp];
         mOpHooks[lnOp] = lpTempNode;
      }
   }
   mHookMutex.unlock();
}

//------------------------------------------------------------------------------
bool CrudRouter::initialize(bool bThreaded)
{
   if (!mCrudMsgQueue.initialize())
   {
      return false;
   }
   
   mpMsgProcThread = Thread::Create(MsgProcThread, this);
   if (mpMsgProcThread == NULL)
   {
      return false;
   }
   
   return true;
}

//------------------------------------------------------------------------------
bool CrudRouter::pushMsg(CrudMsg* pMsg)
{
   return mCrudMsgQueue.push(pMsg);
}

//------------------------------------------------------------------------------
bool CrudRouter::hook(Operation op, void* pOwner, CrudCb pFunc)
{
   if (pOwner == NULL || pFunc == NULL) return false;
   
   CbNode* lpNode = new CbNode;
   
   lpNode->pOwner = pOwner;
   lpNode->pFunc  = pFunc;
   lpNode->pNextNode = NULL;
   
   mHookMutex.lock();
   mOpHooks[op] = lpNode;
   mHookMutex.unlock();
   
   return true;
}

//------------------------------------------------------------------------------
bool CrudRouter::unhook(Operation op, void* pOwner)
{
   bool lbFound = false;
   
   if (pOwner == NULL) return false;
   
   mHookMutex.lock();
   if (mOpHooks[op] != NULL)
   {
      CbNode* lpCurNode = mOpHooks[op];
      CbNode** lppPrevNode = &mOpHooks[op];

      while (lpCurNode)
      {
         if (lpCurNode->pOwner == pOwner)
         {
            (*lppPrevNode)->pNextNode = lpCurNode->pNextNode;
            delete lpCurNode;
            lbFound = true;
            break;
         }
      
         *lppPrevNode = lpCurNode;
         lpCurNode = lpNode->pNextNode;
      }
   }
   mHookMutex.unlock();
   
   return lbFound;
}

//------------------------------------------------------------------------------
void CrudRouter::routeMessage(ui32 nTimeoutMs)
{
   ui32     lnRouteCount = 0;
   CrudMsg* lpMsg = NULL;

   if (mCrudMsgQueue.pop(lpMsg, nTimeoutMs))
   {
      if (lpMsg != NULL && lpMsg->valid())
      {
         Operation lOperation = parseOperation(lpMsg->Data()->operation);
         
         if (lOperation < 0 && lOperation >= NumOps) return;
         
         mHookMutex.lock();
         
         CbNode* lpNode = mOpHooks[lOperation];
         while (lpNode)
         {
            if (lpNode->pFunc)
            {
               lpNode->pFunc(lOperation, lpNode->pOwner, lpMsg);
               lnRouteCount++;
            }
            
            lpNode = lpNode->pNextNode;
         }
         
         mHookMutex.unlock();
         
         if (lnRouteCount == 0)
         {
            delete lpMsg;
            lpMsg = NULL;
         }
      }
   }
}

//------------------------------------------------------------------------------
void CrudRouter::MsgProcThread(ThreadArg* pArg)
{
   ((CrudRouter*)pArg->pUserData)->msgProcThread(pArg);
}

//------------------------------------------------------------------------------
void CrudRouter::msgProcThread(ThreadArg* pArg)
{      
   while (!pArg->stopSignalled())
   {
      routeMessage(100);
   }
}

//------------------------------------------------------------------------------
CrudRouter::Operation CrudRouter::parseOperation(const char* pOpStr)
{
   Operation lOperation = NumOps;
   
   if (strncmp(pOpStr, CreateMsg::OpString) == 0)
   {
      lOperation = Create;
   }
   else if (strncmp(pOpStr, ReadMsg::OpString) == 0)
   {
      lOperation = Read;
   }
   else if (strncmp(pOpStr, UpdateMsg::OpString) == 0)
   {
      lOperation = Update;
   }
   else if (strncmp(pOpStr, DeleteMsg::OpString) == 0)
   {
      lOperation = Delete;
   }
   
   return lOperation;
}
