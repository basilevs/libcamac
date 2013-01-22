#ifndef LECROY2249_H
#define LECROY2249_H

#include "CamacServer.h"

class LeCroy2249 {
	Camac::Module & _module;
public:
	LeCroy2249(Camac::Module & module);
	~LeCroy2249();
	uint16_t read(uint8_t channel);
	void reset();
};

#endif // LECROY2249_H
