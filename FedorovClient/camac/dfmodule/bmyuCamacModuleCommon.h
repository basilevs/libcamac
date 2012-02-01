#ifndef  _bmyuCamacModuleCommon_h
# define _bmyuCamacModuleCommon_h

#include <camac/dfmodule/base.h>
#include <camac/df/std_lcm.h>

class bmyuCamacModuleCommon : public dfCamacModuleBase
{
	int SettingOpenFlags, UnSettingOpenFlags;

protected:
	inline void SetOpenFlags(int set, int unset)
	{ SettingOpenFlags = set; UnSettingOpenFlags = unset; 		}

	inline bmyuCamacModuleCommon(
		const char* _type="", const camac_lam_check_method* _lcm=&dfcamac_lcm_none,
		const char* _signature=0, void* _shmptr=0, size_t _shmsize=0 )
		: dfCamacModuleBase( _type, _lcm, _signature, _shmptr, _shmsize )
	{}
	inline ~bmyuCamacModuleCommon();

public:
	inline int Bind(const camac_address& _addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
		{ return dfCamacModuleBase::Bind(_addr, lock_station_timeout,
			(flags & ~UnSettingOpenFlags) | SettingOpenFlags); }
	inline int Unbind(void)
		{ return dfCamacModuleBase::Unbind(); 					}

public:
	inline int Init(void)
		{ return dfCamacModuleBase::Init();						}
	inline dev_test_state Test(void)
		{ return dfCamacModuleBase::Test(); 					}
	inline int Verify(void)
		{ return dfCamacModuleBase::Verify();					}

public:
	inline int   LockStation(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return dfCamacModuleBase::LockStation(timeout); 		}
	inline int UnlockStation(void)
		{ return dfCamacModuleBase::UnlockStation(); 			}

public:
	inline int SetEventsMask(u16_t events_mask)
		{ return dfCamacModuleBase::SetEventsMask(events_mask);	}
	inline int GetEvents	(void) const
		{ return dfCamacModuleBase::GetEvents(); 				}

};

inline bmyuCamacModuleCommon::~bmyuCamacModuleCommon() { Unbind(); };

typedef bmyuCamacModuleCommon Base;
typedef bmyuCamacModuleCommon Base;

#endif	//_bmyuCamacModuleCommon_h

