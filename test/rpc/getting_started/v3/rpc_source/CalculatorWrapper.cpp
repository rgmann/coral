#include "calculator.pb.h"
#include "CalculatorWrapper.h"

using namespace rpctest;

//-----------------------------------------------------------------------------
CalculatorWrapper::CalculatorWrapper()
:  InstanceWrapper(),
   mpInst(NULL)
{
}

//-----------------------------------------------------------------------------
CalculatorWrapper::~CalculatorWrapper()
{
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::initialize(const std::string &params)
{
   mpInst = new Calculator(/* TODO: Pass correct parameters*/);
   return (mpInst != NULL);
}

//-----------------------------------------------------------------------------
bool CalculatorWrapper::destroy(const std::string &params)
{
   if (mpInst)
   {
      delete mpInst;
      mpInst = NULL;
   }
   return true;
}

//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
addDelegate
(InstanceWrapper* pInst, const std::string& input, std::string& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   rpctest::CalculatorAddIn lInMessage;
   rpctest::CalculatorAddOut lOutMessage;

   lInMessage.ParseFromString(input);

   bool lbHasAllFields = true;
   lbHasAllFields &= lInMessage.has_first();
   lbHasAllFields &= lInMessage.has_second();
   if (!lbHasAllFields)
   {
      return MissingParameters;
   }

   lOutMessage.set_result(lpInst->add(lInMessage.first(), lInMessage.second()));
   lOutMessage.SerializeToString(&output);

   return NoException;
}


//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
subDelegate
(InstanceWrapper* pInst, const std::string& input, std::string& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   rpctest::CalculatorSubIn lInMessage;
   rpctest::CalculatorSubOut lOutMessage;

   lInMessage.ParseFromString(input);

   bool lbHasAllFields = true;
   lbHasAllFields &= lInMessage.has_first();
   lbHasAllFields &= lInMessage.has_second();
   if (!lbHasAllFields)
   {
      return MissingParameters;
   }

   lOutMessage.set_result(lpInst->sub(lInMessage.first(), lInMessage.second()));
   lOutMessage.SerializeToString(&output);

   return NoException;
}

//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
mulDelegate
(InstanceWrapper* pInst, const std::string& input, std::string& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   rpctest::CalculatorMulIn lInMessage;
   rpctest::CalculatorMulOut lOutMessage;

   lInMessage.ParseFromString(input);

   bool lbHasAllFields = true;
   lbHasAllFields &= lInMessage.has_first();
   lbHasAllFields &= lInMessage.has_second();
   if (!lbHasAllFields)
   {
      return MissingParameters;
   }

   lOutMessage.set_result(lpInst->mul(lInMessage.first(), lInMessage.second()));
   lOutMessage.SerializeToString(&output);

   return NoException;
}

//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
divDelegate
(InstanceWrapper* pInst, const std::string& input, std::string& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   rpctest::CalculatorDivIn lInMessage;
   rpctest::CalculatorDivOut lOutMessage;

   lInMessage.ParseFromString(input);

   bool lbHasAllFields = true;
   lbHasAllFields &= lInMessage.has_first();
   lbHasAllFields &= lInMessage.has_second();
   if (!lbHasAllFields)
   {
      return MissingParameters;
   }

   lOutMessage.set_result(lpInst->div(lInMessage.first(), lInMessage.second()));
   lOutMessage.SerializeToString(&output);

   return NoException;
}

//-----------------------------------------------------------------------------
RpcException CalculatorWrapper::
getNumOpsDelegate
(InstanceWrapper* pInst, const std::string& input, std::string& output)
{
   CalculatorWrapper* lpInst = dynamic_cast<CalculatorWrapper*>(pInst);
   rpctest::CalculatorGetNumOpsIn lInMessage;
   rpctest::CalculatorGetNumOpsOut lOutMessage;

   lInMessage.ParseFromString(input);

   bool lbHasAllFields = true;
   if (!lbHasAllFields)
   {
      return MissingParameters;
   }

   lOutMessage.set_result(lpInst->getNumOps());
   lOutMessage.SerializeToString(&output);

   return NoException;
}


