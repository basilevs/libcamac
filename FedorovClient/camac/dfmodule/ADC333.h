/*
 * ADC333.h
 *
 *  Created on: 03.07.2012
 *      Author: gulevich
 */

#ifndef ADC333_H_
#define ADC333_H_

#include <vector>
#include "base.h"


class ADC333: public dfCamacModuleBase {
public:
	ADC333();
	using dfCamacModuleBase::Bind;
	using dfCamacModuleBase::Init;
    /** Immediately starts cycling measurement.
     *  Continues until stop signal is received.
     *  The method returns immediately.
     */
    int StartCycle();
    /** Prepares the module to start measurement on start signal.
     *  The process will stop itself when no more space will be available in the buffer.
     *  The method returns immediately.
     */
    int BeginSingleRun();

    //Reads data for channel
    int Read(unsigned channel, std::vector<double> & data);

    /** Returns a period between measurements in microseconds.
     * Returns 0 for external ticks.
     * */
    unsigned GetTickInNanoSeconds();

	//!!! All public methods below stop currently active processing and delete obtained data.

    /** Enables channel readout.
     * Accepts array at least 4 booleans long.
     */
    int EnableChannels(bool channels[]);
    void SetTickInNanoSeconds(unsigned);

	virtual ~ADC333();
private:
	enum {CHAN_COUNT=4};
	struct Parameters {
		struct Channel {
			bool     enabled;
			unsigned scale;     // In 0..3
		};
		Channel channels[CHAN_COUNT];
		unsigned tick;			// In 0..7
		unsigned limits;
		bool writeChannelNumbers;
		bool cycling;
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
    void WaitReady();
    void Halt();
    void Start();
    void UnHalt();
    //Reads data buffer and measurement parameters from module
    void Readout();
};

#endif /* ADC333_H_ */
