
#ifndef  _Coincide_C0308_h
# define _Coincide_C0308_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>

class Coincide_C0308 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;
	typedef ushort COI_ch_t;

public:
	static const char Type[];

private:
	enum
	{
		nChannels 	= 4, maxChannel	= 3, badChannel = ~maxChannel,
		maxAndMask 	= 0xFF,	badAndMask	= ~maxAndMask,
		maxOrMask 	= 0xF,	badOrMask	= ~maxOrMask
	};

	u8_t SettingAndMask[nChannels];
	u8_t SettingOrMask;

	enum
	{
		afSetOR		= CAMAC_MAKE_AF(4,17),
		afGetOR		= CAMAC_MAKE_AF(4, 1)
	};
	static const camac_af_t afSetAND[nChannels];
	static const camac_af_t afGetAND[nChannels];

public:

		Coincide_C0308();
	inline ~Coincide_C0308();

	inline int   Bind(const camac_address& addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
	    { return Base::Bind(addr, lock_station_timeout,
			    flags & ~(CAMAC_OPEN_STATION_CZ_SENSITIVE
				      | CAMAC_OPEN_STATION_LOCK_CZ	) );}
	inline int Unbind(void)
	    { return Base::Unbind();				}

	inline int   Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 		}
	inline int Unlock(void)
		{ return Base::UnlockStation();			}

	int Init(const u8_t *maskAND,  u8_t maskOR);
	int Init(void);
	dev_test_state Test(void);
	int Verify(void);

public:

	int SetMaskAND( u16_t mask, COI_ch_t ch);
	int SetMaskOR ( u16_t mask);

	int GetMaskAND(u8_t* mask,  COI_ch_t ch);
	int GetMaskOR (u8_t* mask);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}
};

inline Coincide_C0308::~Coincide_C0308() { Unbind();}


#endif	//_Coincide_C0308_h
