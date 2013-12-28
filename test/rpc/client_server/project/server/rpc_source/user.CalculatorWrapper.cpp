#include "CalculatorWrapper.h"

using namespace liber::rpc;
using namespace rpc_getting_started;

//-----------------------------------------------------------------------------
void CalculatorWrapper::
add(const Parameters& request, Result& response, RpcException& e)
{
   std::vector<int> values;
   for (int ind = 0; ind < request.values_size(); ind++)
     values.push_back(request.values(ind));
   response.set_result( mCalculator.add( values ) );
   response.set_op_count( mCalculator.getNumOps() );
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
sub(const Parameters& request, Result& response, RpcException& e)
{
   /* TODO: Implement action here. */
   std::vector<int> values;
   for (int ind = 0; ind < request.values_size(); ind++)
     values.push_back(request.values(ind));
   response.set_result( mCalculator.sub( values ) );
   response.set_op_count( mCalculator.getNumOps() );
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
mul(const Parameters& request, Result& response, RpcException& e)
{
   /* TODO: Implement action here. */
   std::vector<int> values;
   for (int ind = 0; ind < request.values_size(); ind++)
     values.push_back(request.values(ind));
   response.set_result( mCalculator.mul( values ) );
   response.set_op_count( mCalculator.getNumOps() );
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
div(const Parameters& request, Result& response, RpcException& e)
{
   /* TODO: Implement action here. */
   std::vector<int> values;
   for (int ind = 0; ind < request.values_size(); ind++)
     values.push_back(request.values(ind));
   response.set_result( mCalculator.div( values ) );
   response.set_op_count( mCalculator.getNumOps() );
}


//-----------------------------------------------------------------------------
void CalculatorWrapper::
getNumOps(const Parameters& request, Result& response, RpcException& e)
{
   /* TODO: Implement action here. */
   response.set_result( mCalculator.getNumOps() );
   response.set_op_count( mCalculator.getNumOps() );
}



