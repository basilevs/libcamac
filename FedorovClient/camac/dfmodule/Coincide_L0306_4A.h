#ifndef  _Coincide_L0306_4A_h
# define _Coincide_L0306_4A_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>

class Coincide_L0306_4A : public dfCamacModuleBase
{

public:
	typedef dfCamacModuleBase Base;
	typedef ushort COI_ch_t;
	static const char Type[];

	enum
	{
		nChannels 	= 4, maxChannel	= 3, badChannel = ~maxChannel,
		maxAndMask 	= 0xFF,	badAndMask	= ~maxAndMask,
		maxOrMask 	= 0xF,	badOrMask	= ~maxOrMask
	};

private:
	__u8 SetingAndMask[nChannels];
	__u8 SetingOrMask;

		enum
	{
		afSetOR		= CAMAC_MAKE_AF(4,17),
		afGetOR		= CAMAC_MAKE_AF(4, 1)
	};
	static const camac_af_t afSetAND[nChannels];
	static const camac_af_t afGetAND[nChannels];

public:

			Coincide_L0306_4A();
	inline ~Coincide_L0306_4A();

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

	int Init(const __u8 *maskAND,  __u8 maskOR);
	int Init(void);
	dev_test_state Test(void);
	int Verify(void);

public:

	//Установить маски
	int SetMaskAND( __u16 mask, COI_ch_t ch);
	int SetMaskOR ( __u16 mask);

	//Прочитать маски
	int GetMaskAND(__u8* mask,  COI_ch_t ch);
	int GetMaskOR (__u8* mask);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}
};

inline Coincide_L0306_4A::~Coincide_L0306_4A() { Unbind();}


#endif	//_Coincide_L0306_4A_h
