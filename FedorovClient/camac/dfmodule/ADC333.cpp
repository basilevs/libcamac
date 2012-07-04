#include "ADC333.h"
#include <assert.h>
#include <limits>
#include <camac/df/std_lcm.h>

using namespace std;

enum {HALT_MASK = 1, BUSY_MASK=1<<3, CYCLIC_MASK=1<<1, START_ADDRESS=65535, OVERFILL_MASK=1<<2};

struct CamacError {
	int code;
};

static void handleCamacCode(int code) {
	if (code & CAMAC_CC_ERRORS) {
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
		Start();
	} catch (CamacError e) {
		return e.code;
	}
	return CAMAC_CC_OK;
}

int ADC333::BeginSingleRun()
{
	try {
		_parameters.cycling = false;
		Start();
	} catch (CamacError & e) {
		return e.code;
	}
	return CAMAC_CC_OK;
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
			if ((code << 13) & 1 ) {
				handleCamacCode(CAMAC_CC_VERIFICATION_ERROR);
			}
			if (code >> 13 & 1) {
				//Overload
				data.push_back(numeric_limits<double>::infinity());
				continue;
			}
			if (_parameters.writeChannelNumbers) {
				if (((code >> 14) & 3) != channel)
					handleCamacCode(CAMAC_CC_VERIFICATION_ERROR);
			}
			data.push_back((code & 0x7F) << _parameters.channels[channel].scale);
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

int ADC333::EnableChannels(bool channels[])
{
	for (int i =0; i < CHAN_COUNT; ++i)
		_parameters.channels[i].enabled = channels[i];
	return CAMAC_CC_OK;
}

ADC333::~ADC333() {
	// TODO Auto-generated destructor stub
}

void ADC333::WriteParameters(const Parameters & p)
{
	WriteStatus(p.GetStatus() | HALT_MASK);
	WriteLimits(p.GetLimits());
}

void ADC333::Reset()
{
	_parameters.writeChannelNumbers = true;
	WriteParameters(_parameters);
	WriteRegister(8, START_ADDRESS);
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

/** Waits for non-buzy state and halts then */
void ADC333::WaitReady()
{

	unsigned status;
	while(true) {
		status = ReadStatus();
		if (!(status & BUSY_MASK))
			break;
		handleCamacCode(WaitLAM(DF_TIMEOUT_PTR_INF));
	}
	WriteStatus(status | HALT_MASK);
}

void ADC333::Halt()
{
	WriteStatus(ReadStatus()| HALT_MASK);
}

void ADC333::UnHalt()
{
	WriteStatus(ReadStatus() & ~HALT_MASK);
}

void ADC333::Start()
{
	Reset();
	UnHalt();
	camac_af_t af = CAMAC_MAKE_AF(0, 25);
	handleCamacCode(AF(af));
}

void ADC333::Readout()
{
	WaitReady();
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

bool ADC333::Parameters::SetStatus(unsigned  status)
{
	if (status & ~0x1F)
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
	assert((rv & ~0x1F) == 0);
	return true;
}

bool ADC333::Parameters::SetLimits(unsigned  limits)
{
	if (limits & ~0x7F)
		return false;
	for (int i = 0; i < CHAN_COUNT; ++i) {
		channels[i].scale = (limits >> (i * 2)) & 3;
	}
	tick = (limits >> 8) & 7;
	return true;
}

unsigned ADC333::Parameters::GetLimits() const
{
	unsigned rv = 0;
	for (int i = 0; i < CHAN_COUNT; ++i) {
		assert(channels[i].scale < 4);
		rv |= channels[i].scale << (i*2);
	}
	assert(tick < 8);
	rv |= tick << 8;
	assert(!(rv & ~0x7F));
	return rv;
}









