/*
$Id: Discriminator_D0302.h,v 1.2 2003/01/15 08:20:24 barnykov Exp $
*/

#ifndef  _Discriminator_D0302_h
# define _Discriminator_D0302_h

#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>

class Discriminator_D0302 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	typedef u32_t DISC_ch_t;
	static const char Type[];
	enum
	{
		nChannels 	= 4, maxChannel	= 3, badChannel = ~maxChannel,
		maxMask 	= 0xFF,	badMask	= ~maxMask,   maxUt = 1285, minUt = 10
	};

private:
	u8_t SetingMask[nChannels];

	static const camac_af_t afSet[nChannels];
	static const camac_af_t afGet[nChannels];

public:
			Discriminator_D0302();
	inline ~Discriminator_D0302();

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

public:
	int Init(const u16_t *mask);
	int Init(u16_t mask, DISC_ch_t ch);
	int Init(void);
	dev_test_state Test(void);
	int Verify(void);

private:
	inline  u16_t UmV2code(float UmV) const
		{ return (UmV > maxUt) ? maxMask : (u16_t)((UmV-10)/5 + 0.5); }
	inline float code2UmV(u16_t code) const
		{ return ( code*5. + 10. ); }

public:
	int SetThreshold(u16_t mask, DISC_ch_t ch);
//	inline int SetThreshold(float UmV, DISC_ch_t ch)
//		{ return SetThreshold( UmV2code(UmV), ch); }


	int GetThreshold(u16_t* mask, DISC_ch_t ch);
	int GetThreshold(float*  UmV, DISC_ch_t ch);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}
};

inline Discriminator_D0302::~Discriminator_D0302() { Unbind();}


#endif	//_Discriminator_D0302_h

