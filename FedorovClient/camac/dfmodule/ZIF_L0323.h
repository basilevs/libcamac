#ifndef  _ZIF_L0323_h
# define _ZIF_L0323_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>

class ZIF_L0323 : public dfCamacModuleBase
{
public:
	typedef dfCamacModuleBase Base;
	typedef ushort ZIF_ch_t;
	static const char Type[];
	enum {
		nChannels = 2, maxChannel = 1, badChannels = ~maxChannel,
		DurationMask = 0xFF, badDuration = ~DurationMask,
        DurationLength = 8,
		ScalerMask = 0x3, ScalerLength = 2, ScalerStep = 2,
		ScalerProhibitValue = 3 };
	enum {minDuration = 0, maxDuration = 102000, DurationStep = 100};

private:
					__u8   Scaler;
					__u8   Duration[nChannels];
	static const camac_af_t  afRead[nChannels];
	static const camac_af_t afWrite[nChannels];
	static const camac_af_t afScalerRead  = CAMAC_INIT_AF(2, 1);
	static const camac_af_t afScalerWrite = CAMAC_INIT_AF(2,17);

public:
			ZIF_L0323();
	inline ~ZIF_L0323();

	inline int Bind(const camac_address& addr,
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

public:
	int Init(void);
	int Init(const __u8* Dur);
	int Init(__u8 Dur, ZIF_ch_t ch);
	dev_test_state Test(void);
//	dev_test_state Test(ZIF_ch_t ch);
	int Verify(void);

public:
	int SetDuration(ZIF_ch_t channel, __u16 duration);
	int GetDuration(ZIF_ch_t channel, __u16* data);

	int SetScaler(ZIF_ch_t channel, __u16  scale);
	int GetScaler(ZIF_ch_t channel, __u16* scale);

	int SetTime(ZIF_ch_t channel, float  scale);
	int GetTime(ZIF_ch_t channel, float* scale);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}

};

inline ZIF_L0323::~ZIF_L0323() { Unbind(); }


#endif	//_ZIF_L0323_h
