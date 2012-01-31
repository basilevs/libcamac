#include <iostream>
#include <iomanip>
#include <CamacServer.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>

using namespace Camac;
using namespace std;

int main(int argc, char * argv[]) {
	if (argc < 4)
		return 1;
	Server * server = Server::getDefault();
	assert(server);
	Interface * interface = server->getInterface(atoi(argv[1]));
	assert(interface);
	Crate * crate = interface->getCrate(atoi(argv[2]));
	assert(crate&&"Failed to open crate device");
	Module * module = crate->getModule(atoi(argv[3]));
	assert(module);
	timeval was, current;
	unsigned d;
	AF af(0,0);
	const long count = 100000;
	cout.flags(cout.flags()|cout.fixed);
	cout.precision(1);
	while (true) {
		if (gettimeofday(&was, 0)) {
			cerr << "gettimeofday failed" << endl;
			break;
		}
		for (long i = 0; i < count; ++i) {
			Result res = module->AFR(af, d);
			if (res & (~CC_NOT_Q)) {
				printResult(cerr, res) << endl;
				return 1;
			}
        }
		if (gettimeofday(&current, 0)) {
			cerr << "gettimeofday failed" << endl;
			break;
		}
		long sec = current.tv_sec - was.tv_sec;
		long usec = current.tv_usec - was.tv_usec;
		double delta = double(sec) + double(usec)*1e-6;
		cout << double(count)/delta << " Hz\t" << delta/count*1e6 << " mks" << endl;
	}
	return 0;
}
