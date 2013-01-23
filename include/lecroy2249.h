#ifndef LECROY2249_H
#define LECROY2249_H

#include "CamacServer.h"

class LeCroy2249 {
	Camac::Module & _module;
	bool _ignoreQ;
public:
	enum {CHANNEL_COUNT=12};
	LeCroy2249(Camac::Module & module);
	void ignoreQ(bool ignore);
	~LeCroy2249();
	uint16_t read(uint8_t channel);
	bool checkLAM();
	void reset();
	bool waitLAM(Camac::Timeout & t) {return _module.waitLAM(t);}
};

#endif // LECROY2249_H
