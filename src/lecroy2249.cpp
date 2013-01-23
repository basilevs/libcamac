#include "../include/lecroy2249.h"
#include <stdexcept>


using namespace std;
using namespace Camac;

LeCroy2249::LeCroy2249(Module& module):
	_module(module)
{
	AF enableLAM(0, 26);
	Result rc = _module.AFC(enableLAM);
	if (rc != CC_NOT_Q)
		throw runtime_error("Bad camac response while resetting module LeCroy2249");
}

void LeCroy2249::ignoreQ(bool ignore) {_ignoreQ = ignore;}

uint16_t LeCroy2249::read(uint8_t channel)
{
	AF af(channel, 0);
	uint16_t rv = 0;
	Result rc = _module.AFR(af, rv);
	if (_ignoreQ)
		rc &= ~CC_NOT_Q;
	if (rc) {		
		throw runtime_error("Bad camac response while reading module LeCroy2249");
	}
	return rv;
}

void LeCroy2249::reset()
{
	AF af(0, 9);
	Result rc = _module.AFC(af);
	if (rc != CC_NOT_Q)
		throw runtime_error("Bad camac response while resetting module LeCroy2249");
}

LeCroy2249::~LeCroy2249()
{
}

