// HVS_B0308 rip-off with one channel logic only

#ifndef  _HVS_B0307_h
# define _HVS_B0307_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <dfcommon/bool.h>

class HVS_B0307 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	static const char Type[];
	enum { nVoltageBits = 12,
	VoltageMask=0xfff, OnMask=0x1000, OverloadMask=0x6000, PolarityMask=0x8000,
	CodeMask=0xffff };
	static const bool ON = true;
	static const bool OFF = false;

private:
    __u16 Code;
	static const camac_af_t afRead;
	static const camac_af_t afWrite;
	static const camac_af_t afOn;
	static const camac_af_t afOff;
	enum {
		afCheckLAM = CAMAC_MAKE_AF(0, 8),
		afClearLAM = CAMAC_MAKE_AF(0,10)
	};

public:

			HVS_B0307();
	inline ~HVS_B0307();

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
    int Init(__u16 v);
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
	int Turn(bool on);

	int SetVoltage(const __u16 v);
	int ResetOverload(void);

	int ReadCode(__u16* pcode);
	int Update(void);

	inline __u16 GetCode(void) const { return Code; }
	inline __u16 GetVoltage(void) const { return Voltage(Code); }
	inline int GetPolarity(void) const { return Polarity(Code); }
	inline bool IsOverload(void) const { return Overload(Code); }
	inline bool IsOn(void) const { return On(Code); }

	int ReadVoltage(__u16* v);

	int ReadState(bool* on, bool* over=0);

	int ReadOverload(bool* over);
	
	int ReadOverloadValue(int &overload_code);	// added this for DCcheck

};

inline HVS_B0307::~HVS_B0307() { Unbind();}

#endif	//_HVS_B0307_h
