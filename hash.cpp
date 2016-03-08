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

void checkisprime_2(const Value** args, Value* res, void*)
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
		ss << " :prime";
	else
 		ss << " :not prime";

    res->setString(ss.str().c_str());

}

REGISTER_FUNCTION(isprime, list_of("int64"), "string", checkisprime_2);