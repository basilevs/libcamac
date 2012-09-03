#ifndef ADC333_H_
#define ADC333_H_

#include <inttypes.h>
#include <vector>
#include <stdexcept>
#include <base.h>
#include "tools/CamacErrorPrinter.h"

/**
 * ADC333 protocol implementation.
 * http://kedr-wiki.inp.nsk.su/index.php/ADC333
 *
 * Usage:
 * Bind()
 * Init()
 * SetTickInNanoSeconds()
 * Set
 */
class ADC333: public dfCamacModuleBase {
public:
	struct CamacError: public std::runtime_error {
		int _code;
		CamacError(int code, const std::string & message) : runtime_error(message), _code(code) {}
		int code() const {return _code;}
	};
	enum {CHAN_COUNT=4};
	ADC333();
	using dfCamacModuleBase::Bind;
	using dfCamacModuleBase::Init;
	#ifndef CAMAC_MODULE_MOCK
	using dfCamacModuleBase::GetFD;
	#endif
	/** Immediately starts cycling measurement.
	*  Continues until stop signal is received.
	*  Hardware signal STOP stops the measurement.
	*  The method returns immediately.
	*/
	void StartCycle();
	/** Prepares the module to start measurement on start signal.
	*  The process will stop itself when no more space will be available in the buffer.
	*  Actual measurement starts on hardware signal START.
	*  The method returns immediately.
	*/
	void StartSingleRun();

	//Reads data for channel
	//If channel is disabled of invalid throws CamacError(CAMAC_CC_INVALID_ARG)
	//Output vector is cleared and filled with measurements in millivolts.
	void Read(unsigned channel, std::vector<double> & data);

	/** Returns a period between measurements in microseconds.
	* Returns 0 for external ticks.
	* */
	unsigned GetTickInNanoSeconds() const;
	void SetTickInNanoSeconds(unsigned);

	/** Enables channel readout.
	* Accepts array of at least CHAN_COUNT unsigned numbers.
	* Following values are valid:
	* 0 - disable channel
	* 1..4 - set gain mode
	*/
	int EnableChannels(unsigned gains[CHAN_COUNT]);

	bool IsBusy();

	void Stop();
	
	//Imitates hardware START signal.
	void Trigger();
	
	bool CheckLAM();
		
	virtual ~ADC333();
private:
	void Halt();
	struct Parameters {
		struct Channel {
			bool     enabled;
			unsigned gain;     // Gain mode from 0..3
			Channel();
		};
		Channel channels[CHAN_COUNT];
		unsigned tick;			// In 0..7
		bool writeChannelNumbers;
		bool cycling;
		Parameters();
		bool SetStatus(unsigned status);
		bool SetLimits(unsigned limits);
		unsigned GetStatus() const;
		unsigned GetLimits() const;
	};
	Parameters _parameters;
	void WriteParameters(const Parameters & p);
	void ReadParameters(Parameters & p);
	std::vector<uint16_t> _buffer;
	void Reset();
	void WriteStatus(unsigned status);
	unsigned ReadStatus();
	void WriteLimits(unsigned value);
	unsigned ReadLimits();
	void WriteRegister(unsigned index, unsigned value);
	unsigned ReadRegister(unsigned index);
	void UnHalt();
	//Reads data buffer and measurement parameters from module
	void Readout();
};

#endif /* ADC333_H_ */
