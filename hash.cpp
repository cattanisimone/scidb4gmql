#include <vector>
#include <boost/assign.hpp>
#include <math.h>

#include "query/Operator.h"
#include "query/FunctionLibrary.h"
#include "query/FunctionDescription.h"
#include "query/TypeSystem.h"

using namespace std;
using namespace scidb;
using namespace boost::assign;

void dimension_hash(const Value** args, Value* res, void*)
{
	if(args[0]->isNull()){
    	res->setNull(args[0]->getMissingReason());
    	return;
  	}

  	std::string value = args[0]->getString();

	if(value.size() > 10)
		res->setInt64(1);
	else
 		res->setInt64(0);
}

REGISTER_FUNCTION(dim_hash, list_of("int64"), "string", dimension_hash);