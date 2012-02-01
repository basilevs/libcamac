
#ifndef  _TDC_C0301_h
# define _TDC_C0301_h

#include <camac/dfmodule/base.h>
#include <tools/bmyu_out_err.h>
#include <linux/types.h>

class TDC_C0301 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;
	typedef u32_t TDC_ch_t;
	
	TDC_C0301(const TDC_C0301&);
	TDC_C0301& operator=(const TDC_C0301&);
public:
	static const char Type[];
	enum
	{
		nCh  = 4,	maxCh  = 3,	badCh  = ~maxCh,
		maxCount = 0xFFF//,	maxLAM = 0xFF,	badLAM = ~maxLAM
	};

private:
	static 	const camac_af_t afRead[nCh];
	static	const camac_af_t afReadClear[nCh];
	static 	const camac_af_t afCheckLAM=CAMAC_INIT_AF(0, 8);
	static 	const camac_af_t afClear=CAMAC_INIT_AF(0, 10);

public:
			TDC_C0301();
	inline ~TDC_C0301();

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
	int			CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);	//doesn't work
	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
    	{ return   Base::Lock(CAMAC_LOCK_CZ, timeout);	}
	inline int UnLockCZ(void)
		{ return Base::Unlock(CAMAC_LOCK_CZ);			}

public:
	int Reset(void);	//clear LAM and data in all channels


	//-------------------------------

	inline int Read(TDC_ch_t channel, __u16* data)
	{ return ( channel & badCh ) ?
						CAMAC_CC_INVALID_ARG :
								AF( afRead[channel], data ) | CompleteLAM();		}

	inline int ReadClear(TDC_ch_t channel, __u16* data)		// clear data in one specific channel
	{ return ( channel & badCh ) ?							// note that LAM will still be active
						CAMAC_CC_INVALID_ARG :
								AF( afReadClear[channel], data ) | CompleteLAM();	}

	inline int CheckLAM(void)	{ return AF( afCheckLAM );	}
	inline int ClearLAM(void)	{ return AF( afClear );	}

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

inline     TDC_C0301::~TDC_C0301()
	{ Unbind(); 									}

#endif	//_TDC_C0301_h
