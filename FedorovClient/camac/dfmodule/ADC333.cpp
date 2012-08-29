#include "ADC333.h"
#include <assert.h>
#include <limits>
#include <iostream> //clog
#include <camac/df/std_lcm.h>

using namespace std;

enum {HALT_MASK = 1, BUSY_MASK=1<<3, CYCLIC_MASK=1<<1, START_ADDRESS=65535, OVERFILL_MASK=1<<2};

#ifndef CAMAC_MODULE_MOCK
enum {CAMAC_CC_OK = 0};
#endif


struct CamacError {
	int code;
};

static void handleCamacCode(int code) {
	if (code & CAMAC_CC_ERRORS & ~CAMAC_CC_NOT_Q) {
		throw (CamacError){code};
	}
}

ADC333::ADC333():
	dfCamacModuleBase("ADC333", &dfcamac_lcm_a0_f8_q)
{

}

int ADC333::StartCycle()
{
	try {
		_parameters.cycling = true;
		Reset();
		UnHalt();
		handleCamacCode(Trigger());
	} catch (CamacError e) {
		return e.code;
	}
	return CAMAC_CC_OK;
}

int ADC333::StartSingleRun()
{
	try {
		_parameters.cycling = false;
		Reset();
		UnHalt();
	} catch (CamacError & e) {
		return e.code;
	}
	return CAMAC_CC_OK;
}

int ADC333::Trigger() 
{
	const camac_af_t af = CAMAC_MAKE_AF(0, 25);
	return AF(af);	
}

int ADC333::Stop() 
{
	try {
		Halt();
	} catch(CamacError & e) {
		return e.code;
	}
	return 0;
}

int ADC333::Read(unsigned  channel, std::vector<double> & data)
{
	try {
		if (_buffer.empty())
			Readout();
		unsigned chanCount = 0;
		unsigned chanIdx =  CHAN_COUNT;
		for (unsigned i = 0; i < CHAN_COUNT; ++i) {
			if (_parameters.channels[i].enabled) {
				if (i==channel) {
					chanIdx = chanCount;
				}
				chanCount ++;
			} else {
				if (i == channel)
					handleCamacCode(CAMAC_CC_INVALID_ARG);
			}
		}
		data.clear();
		for (unsigned i = chanIdx; i < _buffer.size(); i+=chanCount) {
			uint16_t code = _buffer[i];
			if ((code >> 13) & 1 ) {
				handleCamacCode(CAMAC_CC_VERIFICATION_ERROR);
			}
			if ((code >> 12) & 1) {
				//Overload
				data.push_back(numeric_limits<double>::infinity());
				continue;
			}
			if (_parameters.writeChannelNumbers) {
				if (unsigned((code >> 14) & 3) != channel)
					handleCamacCode(CAMAC_CC_VERIFICATION_ERROR);
			}
			int value = code & 0xFFF;
			value -= 0x7FF;
			double gain = 1 << _parameters.channels[channel].gain;
			data.push_back(double(value) * gain );
		}
	} catch(CamacError &e) {
		return e.code;
	}
	return CAMAC_CC_OK;
}

static unsigned tickToNanoSeconds[8] = {500, 1000, 2000, 4000, 8000, 16000, 32000, 0};

unsigned ADC333::GetTickInNanoSeconds() const
{
	if (_parameters.tick > 7)
		handleCamacCode(CAMAC_CC_VERIFICATION_ERROR);
	return tickToNanoSeconds[_parameters.tick];
}

void ADC333::SetTickInNanoSeconds(unsigned ns)
{
	for (unsigned i = 0; i < 8; ++i) {
		if (tickToNanoSeconds[i] == ns) {
			_parameters.tick = i;
			return;
		}
	}
	handleCamacCode(CAMAC_CC_INVALID_ARG);
}

int ADC333::EnableChannels(unsigned channels[])
{
	for (int i =0; i < CHAN_COUNT; ++i) {
		assert(channels[i] >=0);
		assert(channels[i] < 5);
		bool enabled = channels[i] > 0;
		_parameters.channels[i].enabled = enabled;
		if (enabled)
			_parameters.channels[i].gain = channels[i] - 1;
		else 
			_parameters.channels[i].gain = 0;
	}
	return CAMAC_CC_OK;
}

ADC333::~ADC333() {
	// TODO Auto-generated destructor stub
}

void ADC333::WriteParameters(const Parameters & p)
{
	Halt();
	WriteLimits(p.GetLimits());
	WriteStatus(p.GetStatus());
}

void ADC333::Reset()
{
	_parameters.writeChannelNumbers = true;
	WriteParameters(_parameters);
	WriteRegister(8, START_ADDRESS);
	assert(ReadRegister(8) == START_ADDRESS);
	const camac_af_t clearLAM = CAMAC_MAKE_AF(0, 10);
	handleCamacCode(AF(clearLAM));
	const camac_af_t unblockLAM = CAMAC_MAKE_AF(0, 26);
	handleCamacCode(AF(unblockLAM));
	_buffer.clear();
}

void ADC333::WriteStatus(unsigned  status)
{
	WriteRegister(9, status);
}

unsigned ADC333::ReadStatus()
{
	return ReadRegister(9);
}

void ADC333::WriteLimits(unsigned  value)
{
	WriteRegister(10, value);
}


unsigned ADC333::ReadLimits()
{
	return ReadRegister(10);
}

unsigned ADC333::ReadRegister(unsigned index) {
	const camac_af_t af = CAMAC_MAKE_AF(index, 0);
	u16_t tmp = 0;
	int rv = AF(af, &tmp);
	handleCamacCode(rv);
	return tmp;
}

void ADC333::WriteRegister(unsigned  index, unsigned value)
{
	const camac_af_t af = CAMAC_MAKE_AF(index, 16);
	u16_t tmp = value;
	handleCamacCode(AF(af, &tmp));
}

void ADC333::ReadParameters(Parameters & p)
{
	unsigned status=ReadStatus(), limits = ReadLimits();
	if (!p.SetStatus(status)) {
		throw (CamacError){CAMAC_CC_VERIFICATION_ERROR};
	}
	if (!p.SetLimits(limits)) {
		throw (CamacError){CAMAC_CC_VERIFICATION_ERROR};
	}
}

void ADC333::Halt()
{
	WriteStatus(ReadStatus()| HALT_MASK);
}

void ADC333::UnHalt()
{
	WriteStatus(ReadStatus() & ~HALT_MASK);
}


int ADC333::IsBusy()
{
	try {
		if (ReadStatus() & BUSY_MASK)
			return CAMAC_CC_BOOL;
		return CAMAC_CC_OK;
	} catch (CamacError & e) {
		return e.code;
	}
}

void ADC333::Readout()
{
	Halt();
	unsigned status = ReadStatus();
	ReadParameters(_parameters);
	long stop_position = -1;
	if (!(status & CYCLIC_MASK) || !(status & OVERFILL_MASK)) {
		stop_position=ReadRegister(8); // Current position
		WriteRegister(8, START_ADDRESS);
	}
	_buffer.clear();
	for (long i = START_ADDRESS; i > stop_position; --i) {
		_buffer.push_back(ReadRegister(0));
	}
}

bool ADC333::CheckLAM() 
{
	const camac_af_t af = CAMAC_MAKE_AF(0, 8);
	int rv = AF(af);
	handleCamacCode(rv);
	if (rv & CAMAC_CC_NOT_Q)
		return false;
	return true;
}

ADC333::Parameters::Parameters():
	tick(6), //32 us
	writeChannelNumbers(true),
	cycling(false)
{
}

bool ADC333::Parameters::SetStatus(unsigned  status)
{
	if (status & ~0x1FF)
		return false;
	writeChannelNumbers = bool(status >> 8 & 1);
	cycling = bool(status >> 1 & 1);
	for (int chIdx = 0; chIdx < CHAN_COUNT; chIdx++) {
		channels[chIdx].enabled = status >> (4+chIdx) & 1;
	}
	return true;
}

unsigned ADC333::Parameters::GetStatus() const
{
	unsigned rv = 0;
	if (writeChannelNumbers)
		rv |= 1 << 8;
	if (cycling)
		rv |= 1 << 1;
	for (int chIdx = 0; chIdx < CHAN_COUNT; chIdx++) {
		if (channels[chIdx].enabled)
			rv |= 1 << (4+chIdx);
	}
	assert((rv & ~0x1FF) == 0);
	return rv;
}

bool ADC333::Parameters::SetLimits(unsigned  limits)
{
	if (limits & ~0x7FF)
		return false;
	for (int i = 0; i < CHAN_COUNT; ++i) {
		channels[i].gain = (limits >> (i * 2)) & 3;
	}
	tick = (limits >> 8) & 7;
	return true;
}

unsigned ADC333::Parameters::GetLimits() const
{
	unsigned rv = 0;
	for (int i = 0; i < CHAN_COUNT; ++i) {
		assert(channels[i].gain < 4);
		rv |= channels[i].gain << (i*2);
	}
	assert(tick < 8);
	rv |= tick << 8;
	assert(!(rv & ~0x7FF));
	return rv;
}

ADC333::Parameters::Channel::Channel():
		enabled(false),
		gain(0) //Lowest gain possible to maximize voltage range
{
}











