
#ifndef  _GTP_B0609_h
# define _GTP_B0609_h

#include <camac/dfmodule/base.h>
//#include <camac/df/wait_lam.h>
#include "tools/bmyu_out_err.h"
#include <linux/types.h>

class GTP_B0609 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	static const char Type[];

private:
	static const camac_af_t afRead  = CAMAC_MAKE_AF(0, 0);	//i'm not sure about this one
	static const camac_af_t afWrite = CAMAC_MAKE_AF(0, 16);
	static const camac_af_t afStart = CAMAC_MAKE_AF(0, 24);

public:
			GTP_B0609();
	inline ~GTP_B0609();

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

	int			CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);
	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
    	{ return	Base::Lock(CAMAC_LOCK_CZ, timeout);	}
	inline int UnLockCZ(void)
		{ return	Base::Unlock(CAMAC_LOCK_CZ);		}

	int Init(void);
	dev_test_state Test(void);
	int Verify(void)	{ return Base::Verify(); 	}

private:
	inline u16_t ConvertTime(__u16 ext_time)			// my formula values: 25,3; 33,6
														// new: 22,9; 36
		{ return static_cast<u16_t>((ext_time-5)/36); }	//to ensure the code values
									// (ext_time-28)/36 - convert formula courtesy Basok Ivan
									// (ext_time-10)/tau-1  -  old one
public:
	int Reset(void);

	inline int Set(__u16 ext_time)		//time in nsec
		{ u16_t dummy=ConvertTime(ext_time);
		return AF( afWrite, &dummy );		}

	inline int Read(__u16* data)
		{ return AF( afRead, data );		}

	inline int Start(void)
		{ return AF( afStart );								}

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}
};

inline     GTP_B0609::~GTP_B0609()
	{ Unbind(); 									}

#endif	//_GTP_B0609_h
