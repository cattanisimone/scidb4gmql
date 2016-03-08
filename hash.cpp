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
	if(args[0]->isNull())
  	{
    	res->setNull(args[0]->getMissingReason());
    	return;
  	}
  	
  	int64_t l;
  	char buf[8];
  	memset(buf,0,8);
  	snprintf(buf,8,"%s",args[0]->getString());
    
  	l = 42;

    res->setInt64(l)

}

REGISTER_FUNCTION(dim_hash, list_of("string"), "int64", dimension_hash);