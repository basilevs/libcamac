#ifndef _DelayPulseShaper_L0323_h
#define _DelayPulseShaper_L0323_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>

class DelayPulseShaper_L0323 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;
	typedef ushort DPSh_ch_t;

public:
	static const char Type[];

	enum { 	nChannels = 2, maxChannel = 1, badChannels = ~maxChannel,
			maxDuration = 0xFF, badDuration = ~maxDuration };

private:
					__u8   Duration[nChannels];
	static const camac_af_t  afRead[nChannels];
	static const camac_af_t afWrite[nChannels];

public:
			DelayPulseShaper_L0323();
	inline ~DelayPulseShaper_L0323();

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
	int Init(__u8 Dur, DPSh_ch_t ch);
	dev_test_state Test(void);
//	dev_test_state Test(DPSh_ch_t ch);
	int Verify(void);

public:

	//Установить длительность импульса
	//Возвращает camac_сс_bits.
	int SetDuration(DPSh_ch_t channel, __u16 duration);

	//Прочитать длительность
	//Возвращает camac_return_code_bits, CAMAC_RC_NO_Q при неготовности.
	//isbad(*data) при перегрузке.
	int GetDuration(DPSh_ch_t channel, __u16* data);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}

};

inline DelayPulseShaper_L0323::~DelayPulseShaper_L0323() { Unbind(); }


#endif	//_DelayPulseShaper_L0323_h
