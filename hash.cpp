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
	int64_t i,n;
	double r;
	stringstream ss;

    n = args[0]->getInt64();
	r = 0;
	ss << n;

	if(n>1)
		for(i=2;i<=sqrt(n);i++)
			if(n%i==0)
			{
				r = 1;
				break;
			}

	if(r==0 && n>1)
		res->setInt64(1);
	else
 		res->setInt64(0);

}

REGISTER_FUNCTION(dim_hash, list_of("int64"), "int64", dimension_hash);