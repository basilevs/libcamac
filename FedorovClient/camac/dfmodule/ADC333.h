#ifndef ADC333_H_
#define ADC333_H_

#include <inttypes.h>
#include <vector>
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
	int StartCycle();
	/** Prepares the module to start measurement on start signal.
	*  The process will stop itself when no more space will be available in the buffer.
	*  Actual measurement starts on hardware signal START.
	*  The method returns immediately.
	*/
	int StartSingleRun();

	//Reads data for channel
	//If channel is disabled of invalid returns CAMAC_CC_INVALID_ARG
	//Output vector is clread and filled with measurements in millivolts.
	int Read(unsigned channel, std::vector<double> & data);

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

	/// Returns CAMAC_CC_BOOL if the module is busy.
	int IsBusy();

	int Stop();
	
	//Imitates hardware START signal.
	int Trigger();
	
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
