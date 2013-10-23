#ifndef TRPC_SERVER_RESOURCE_H
#define TRPC_SERVER_RESOURCE_H

#include "RpcServerResource.h"

template <class T>
class TRpcServerResource : public RpcServerResource
{
public:

   TRpcServerResource(const std::string &classname);
   
protected:
   
   bool isValidInstance(int nInstId);
   
   T* getInstance(int instId);
   
   bool construct(const RpcObject &input, RpcObject &output);
   
   bool destroy(const RpcObject &input, RpcObject &output);
   
   bool invoke(const RpcObject &input, RpcObject &output);
   
   virtual bool processCommand(T* pInst,
                               const RpcObject &input,
                               RpcObject &output) = 0;
   
   virtual T* createInstance(const Structure &params) = 0;
   
private:
   
   //std::map<int, T*> mInstances;
};


//------------------------------------------------------------------------------
template <class T>
TRpcServerResource<T>::TRpcServerResource(const std::string &classname)
: RpcServerResource(classname)
{
}

//------------------------------------------------------------------------------
template <class T>
bool TRpcServerResource<T>::invoke(const RpcObject &input, RpcObject &output)
{
   return processCommand(getInstance(input.getInstanceId()), input, output);
}

//------------------------------------------------------------------------------
template <class T>
T* TRpcServerResource<T>::getInstance(int instId)
{
   T* lpInstance = NULL;
   
   if (mInstances.count(instId) != 0)
   {
      lpInstance = mInstances[instId];
   }
   
   return lpInstance;
}

//------------------------------------------------------------------------------
template <class T>
bool TRpcServerResource<T>::isValidInstance(int instId)
{
   return (getInstance(instId) != NULL);
}

//------------------------------------------------------------------------------
template <class T>
bool TRpcServerResource<T>::construct(const RpcObject &input,
                                      RpcObject &output)
{
   Structure lParamList;
   
   int lInstanceId = getNextInstanceId();
      
   if (mInstances.count(lInstanceId) != 0)
   {
      exception(InstIdAssignmentErr, input, output);
      return false;
   }
   
   input.getParams(lParamList);
   mInstances[lInstanceId] = createInstance(lParamList);
   
   input.getResponse(output, RpcReturnValue(lInstanceId));

   return true;
}

//------------------------------------------------------------------------------
template <class T>
bool TRpcServerResource<T>::destroy(const RpcObject &input,
                                    RpcObject &output)
{
   if (mInstances.count(input.getInstanceId()) == 0)
   {
      exception(InvalidInstanceId, input, output);
      return false;
   }
   
   delete mInstances[input.getInstanceId()];
   mInstances.erase(input.getInstanceId());
   
   input.getResponse(output);
   
   return true;
}

#endif // TRPC_SERVER_RESOURCE_H
