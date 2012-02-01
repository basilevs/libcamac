
#ifndef  _HVS_B0308_h
# define _HVS_B0308_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <dfcommon/bool.h>

class HVS_B0308 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	static const char Type[];
	enum { nChannels = 2, nVoltageBits = 12,
	VoltageMask=0xfff, OnMask=0x1000, OverloadMask=0x6000, PolarityMask=0x8000,
	CodeMask=0xffff };
	static const bool ON = true;
	static const bool OFF = false;

private:
    __u16 Code[nChannels];
	static const camac_af_t afRead[nChannels];
	static const camac_af_t afWrite[nChannels];
	static const camac_af_t afOn[nChannels];
	static const camac_af_t afOff[nChannels];
	enum {
		afCheckLAM = CAMAC_MAKE_AF(0, 8),
		afClearLAM = CAMAC_MAKE_AF(0,10)
	};

public:

			HVS_B0308();
	inline ~HVS_B0308();

	inline int   Bind(const camac_address& addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
	{ return Base::Bind(addr, lock_station_timeout,
						flags &	~( 	CAMAC_OPEN_STATION_CZ_SENSITIVE
							  |		CAMAC_OPEN_STATION_LOCK_CZ 		) );	}
	inline int Unbind(void)
		{ return Base::Unbind(); 					}

	inline int   Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 		}
	inline int Unlock(void)
		{ return Base::UnlockStation(); 			}

	int Init(void);
    int Init(__u16* v);
	dev_test_state Test(void);
	int Verify(void);

public:
	int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); }
	int GetEvents    (void) const
		{ return Base::GetEvents(); }


private:
	inline __u16 on2mask(const bool on) const { return on ? OnMask : 0; }
	inline __u16 Voltage(const __u16 code) const
		{ return (__u16)VoltageMask&code; }
	inline bool On(const __u16 code) const
		{ return (OnMask&code) ? true : false; }
	inline bool Overload(const __u16 code) const
		{ return (OverloadMask&code) ? true : false; }
	inline int Polarity(const __u16 code) const
		{ return ((__u16)PolarityMask&code) ? 1 : -1; }

public:
	int Turn(size_t channel, bool on);

	int SetVoltage(size_t channel, const __u16 v);
	int ResetOverload(size_t channel);

	int ReadCode(size_t channel, __u16* pcode);
	int Update(size_t channel);
	int Update(void);

	inline __u16 GetCode(size_t channel) const { return Code[channel]; }
	inline __u16 GetVoltage(size_t channel) const { return Voltage(Code[channel]); }
	inline int GetPolarity(size_t channel) const { return Polarity(Code[channel]); }
	inline bool IsOverload(size_t channel) const { return Overload(Code[channel]); }
	inline bool IsOn(size_t channel) const { return On(Code[channel]); }

	int ReadVoltage(size_t channel, __u16* v);

	int ReadState(size_t channel, bool* on, bool* over=0);

	int ReadOverload(size_t channel, bool* over);

	int ReadOverloadValue(size_t channel, int &overload_code);	// added this for DCcheck
};

inline HVS_B0308::~HVS_B0308() { Unbind();}

#endif	//_HVS_B0308_h
