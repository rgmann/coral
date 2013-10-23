#ifndef CALCULATOR_WRAPPER_H
#define CALCULATOR_WRAPPER_H

#include "InstanceWrapper.h"

class CalculatorWrapper : public InstanceWrapper
{
public:
   
   CalculatorWrapper(const Structure &params) : InstanceWrapper(params)
   {
      mpCalc = new Calculator();
   };
   
   ~CalculatorWrapper()
   {
      delete mpCalc;
   };

   static bool add(InstanceWrapper *pInst, Structure &params, RpcReturnValue &result)
   {
      int lA = 0;
      int lB = 0;
      int lResult = 0;
      
      params.get("a", lA);
      params.get("b", lB);
      lResult = static_cast<CalculatorWrapper*>(pInst)->mpCalc->add(lA, lB);
      result.set(lResult);
      return true;
   };
   static bool sub(InstanceWrapper *pInst, Structure &params, RpcReturnValue &result)
   {
      int lA = 0;
      int lB = 0;
      int lResult = 0;
      
      params.get("a", lA);
      params.get("b", lB);
      lResult = static_cast<CalculatorWrapper*>(pInst)->mpCalc->sub(lA, lB);
      result.set(lResult);
      return true;
   };
   static bool mul(InstanceWrapper *pInst, Structure &params, RpcReturnValue &result)
   {
      int lA = 0;
      int lB = 0;
      int lResult = 0;
      
      params.get("a", lA);
      params.get("b", lB);
      lResult = static_cast<CalculatorWrapper*>(pInst)->mpCalc->mul(lA, lB);
      result.set(lResult);
      return true;
   };
   static bool div(InstanceWrapper *pInst, Structure &params, RpcReturnValue &result)
   {
      int lA = 0;
      int lB = 0;
      int lResult = 0;
      
      params.get("a", lA);
      params.get("b", lB);
      lResult = static_cast<CalculatorWrapper*>(pInst)->mpCalc->div(lA, lB);
      result.set(lResult);
      return true;
   };
   static bool getNumOps(InstanceWrapper *pInst, Structure &params, RpcReturnValue &result)
   {
      int lResult = 0;
      
      lResult = static_cast<CalculatorWrapper*>(pInst)->mpCalc->getNumOps();
      result.set(lResult);
      return true;
   };
   
private:
   
   Calculator* mpCalc;
};

#endif // CALCULATOR_WRAPPER_H

