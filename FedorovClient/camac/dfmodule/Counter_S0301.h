#ifndef  _Counter_S0301_h
# define _Counter_S0301_h

#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include "tools/bmyu_out_err.h"
#include <linux/types.h>

class Counter_S0301 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	typedef u32_t COUNT_ch_t;
	static const char Type[];
	enum
	{
		nCh  = 8,	maxCh  = 7,	badCh  = ~maxCh,
		n2Ch = 4,	max2Ch = 3,	bad2Ch = ~max2Ch,
		maxCount = 0xFFFF,	maxLAM = 0xFF,	badLAM = ~maxLAM
	};

private:
	enum
	{
		afLAMcheck		= CAMAC_MAKE_AF(12,8),
		afReadLAM		= CAMAC_MAKE_AF(12,1),
	};

	static 	const camac_af_t      afRead[nCh];
	static	const camac_af_t afReadClear[nCh];
	static 	const camac_af_t afIncrement[nCh];
	static 	const camac_af_t     afClear[nCh];
	static  const struct camac_lam_check_method _lcm;

public:
			Counter_S0301();
	inline ~Counter_S0301();

	inline int Bind(const camac_address& addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
	{ return Base::Bind(addr, lock_station_timeout,
						flags	| CAMAC_OPEN_STATION_CZ_SENSITIVE
								| CAMAC_OPEN_STATION_LOCK_CZ );			}
	inline int Unbind(void)
		{ return Base::Unbind(); 					}

	inline int   Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 		}
	inline int Unlock(void)
		{ return Base::UnlockStation(); 			}

	int Init(void);
	dev_test_state Test(void);
	int Verify(void)	{ return Base::Verify(); 	}

public:
	int			CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);
	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
    	{ return   Base::Lock(CAMAC_LOCK_CZ, timeout);	}
	inline int UnLockCZ(void)
		{ return Base::Unlock(CAMAC_LOCK_CZ);			}

public:

	int Reset(COUNT_ch_t channel);

	int WaitLAM(df_timeout_t* timeout);

	inline int CheckLAM(void)
		{ return ~AF(afLAMcheck) & CAMAC_CC_NOT_Q; 	}

	inline int Read(COUNT_ch_t channel, __u16* data)
	{ return ( channel & badCh ) ?
						CAMAC_CC_INVALID_ARG :
								AF( afRead[channel], data );		}

	inline int ReadClear(COUNT_ch_t channel, __u16* data)
	{ return ( channel & badCh ) ?
						CAMAC_CC_INVALID_ARG :
								AF( afReadClear[channel], data );	}

	int Read2(COUNT_ch_t channel, __u32* data);

	int Read2clear(COUNT_ch_t channel, __u32* data);

	int ReadLAM(__u16* data);

	inline int Increment(COUNT_ch_t channel)
	{ return ( channel & badCh ) ?
						CAMAC_CC_INVALID_ARG :
								AF( afIncrement[channel] );	}

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}

	inline int SetI			(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
		{ return Base::SetAndLockI  (timeout);		}
	inline int ClearI		(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::ClearAndLockI(timeout);		}
	inline int UnlockI		(void)
		{ return Base::UnlockI();					}
};

inline     Counter_S0301::~Counter_S0301()
	{ Unbind(); 									}

#endif	//_Counter_S0301_h

/*
$Log: Counter_S0301.h,v $
Revision 1.3  2003/01/15 08:20:24  barnykov
Some changes...

Revision 1.2  2001/05/29 13:14:01  barnykov
BMYuCamacModuleCommon added.

Revision 1.1.1.1  2001/03/24 14:55:31  barnykov
�������� � ���������� �����������...

Revision 1.1  2001/03/24 02:57:45  barnykov
��� �� Kedr-10.

*/

