#ifndef CALCULATOR_SERVER_STUB_H
#define CALCULATOR_SERVER_STUB_H

#include "Calculator.h"
#include "TRpcServerResource.h"

class CalculatorServerStub : public TRpcServerResource<Calculator>
{
public:

   CalculatorServerStub() : TRpcServerResource<Calculator>("calculator")
   {
   };

protected:
   
   Calculator* createInstance(const Structure &params)
   {
      return new Calculator();
   };
   
   bool processCommand(Calculator* pInst,
                       const RpcObject &input,
                       RpcObject &output)
   {
      i32 pA = 0;
      i32 pB = 0;
      
      Structure lParamList;
      RpcReturnValue lRetVal;
      
      bool lbAllParamsAvail = true;
      bool lbValidMethod = true;
      
      lbAllParamsAvail = input.getParams(lParamList);
      lbAllParamsAvail |= lParamList.get("a", pA);
      lbAllParamsAvail |= lParamList.get("b", pB);
      
//      std::cout << "processCmd: method -> " << input.getMethod() << std::endl
//      << "  a = " << pA << ", b = " << pB << std::endl;
      
      if (!lbAllParamsAvail)
      {
         input.getResponse(output, MissingParameters);
         return false;
      }
      
      if (input.methodEquals("add"))
      {
         lRetVal.set((i32)pInst->add(pA, pB));
      }
      else if (input.methodEquals("sub"))
      {
         lRetVal.set((i32)pInst->sub(pA, pB));
      }
      else if (input.methodEquals("mul"))
      {
         lRetVal.set((i32)pInst->mul(pA, pB));
      }
      else if (input.methodEquals("div"))
      {
         lRetVal.set((i32)pInst->div(pA, pB));
      }
      else if (input.methodEquals("getNumOps"))
      {
         lRetVal.set((i32)pInst->getNumOps());
      }
      else
      {
         input.getResponse(output, UnknownMethod);
         lbValidMethod = false;
      }
      
      if (lbValidMethod)
      {
         input.getResponse(output, lRetVal);
      }

      return lbValidMethod;
   };
};

#endif // CALCULATOR_SERVER_STUB_H
