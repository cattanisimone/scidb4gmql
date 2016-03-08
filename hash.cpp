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
	int hash_tmp = 7;
  	int i;

	if(args[0]->isNull()){
    	res->setNull(args[0]->getMissingReason());
    	return;
  	}

  	std::string value = args[0]->getString();

  	
  	//for(i=0; i<value.size(); i++)
  	hash_tmp = hash_tmp*31 + 25;
	
  	int64_t hash = hash_tmp;
	res->setInt64(hash);
}

REGISTER_FUNCTION(dim_hash, list_of("string"), "int64", dimension_hash);