#include <math.h>
#include <assert.h>

#include "U0302.h"
#include <camac/df/std_lcm.h>

const char U0302::Type[] = "U0302";

U0302::U0302():
	Base(Type, &dfcamac_lcm_none),
	_length(1),
	_enabled(CHANNEL_COUNT, false)
{}

int U0302::Turn(int channel, bool on) {
	assert(channel>=0);
	assert(channel<CHANNEL_COUNT);
	_enabled[channel] = on;
	return update();
}

int U0302::SetLength(float length) {
	assert(length >= 0.00009);
	assert(length <= 4.00001);
	_length = length;
	return update();
}

int U0302::update() {
	unsigned word = 0;
	if (_length > 0) {
		int log10 = int(logf(_length)/logf(10)+4);
		assert(log10 >= 0);
		assert(log10 <= 4);
		float A = 0.0001 * pow(10, log10);
		int B = int(_length/A);
		assert(B > 0);
		assert(B <= 4);
		word |= 2 << log10;
		word |= 1 << (6 + B);
	}
	int rc = 0;
	for (int i = 1; i <= CHANNEL_COUNT; ++i) {
		unsigned short enabledWord = _enabled[i-1] ?  1 : 0;
		enabledWord |= word;
		rc |= AF(CAMAC_MAKE_AF(i, 16), &enabledWord);
	}
	return rc;
}
