#include "CamacServer.h"

#include <stdexcept>
#include <sstream>

using namespace Camac;
using namespace std;

Module & Camac::getModule(Server * pServer, int interface, int crate, int module) {
	if (!pServer)
		throw runtime_error("Null camac server in getModule()");
	Interface * i = pServer->getInterface(interface);
	if (!i) {
		ostringstream temp;
		temp << "No camac interface " << interface;
		throw runtime_error(temp.str());
	}
	Crate * c = i->getCrate(crate);
	if (!c) {
		ostringstream temp;
		temp << "No camac crate " << crate << " in interface " << interface ;
		throw runtime_error(temp.str());
	}
	Module * m = c->getModule(module);
	if (!m) {
		ostringstream temp;
		temp << "No camac module " << module << " in crate " << crate << " of interface " << interface ;
		throw runtime_error(temp.str());
	}
	return *m;
}

ostream & Camac::printResult(ostream & o, Result cc) {
	bool first = true;
	#define TEST_BIT(a) if (CC_##a & cc) {if (!first) o << "|"; first = false; o << #a; cc &= ~CC_##a;}
	if (cc == -1) {
		return o << "UNKNOWN";
	}
	TEST_BIT(BOOL);
	TEST_BIT(SYSCALL_ERROR);
	TEST_BIT(INVALID_ARG);
	TEST_BIT(NOTBINDED);
	TEST_BIT(NOTINITED);

	TEST_BIT(IFACE_TIMEOUT);
	TEST_BIT(DATA_XFER_ERROR);
	TEST_BIT(VERIFICATION_ERROR);
	TEST_BIT(INTERMEDIATE_ERROR);
	TEST_BIT(TEST_FAILED);

	TEST_BIT(NOT_X);
	TEST_BIT(NOT_Q);

	TEST_BIT(USER_ERROR0);
	TEST_BIT(USER_ERROR1);
	TEST_BIT(USER_ERROR2);
	TEST_BIT(USER_ERROR3);

	if (cc) {
		if (first) o << "|";
		o << "UNKNOWN";
		first = false;
	}

	return o;
};

string resultToString(Result res) {
	ostringstream s;
	printResult(s, res);
	return s.str();
}

