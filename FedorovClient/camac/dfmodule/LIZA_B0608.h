#ifndef  _LIZA_B0608_h
# define _LIZA_B0608_h

#include <linux/types.h>
#include <camac/df/std_lcm.h>
#include <camac/dfmodule/base.h>

class LIZA_B0608 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	static const char Type[];
	enum { minDelay = 0, maxDelay = 63, badDelay = ~maxDelay, stepDelay = 1 };

private:
	__u16 setDelay;
	static const camac_af_t afRead;
	static const camac_af_t afWrite;

public:
	 LIZA_B0608() : Base(Type, &dfcamac_lcm_none), setDelay(badDelay) {};
	~LIZA_B0608() { Unbind(); };

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
	int Init(__u16 Delay);
	dev_test_state Test(void);
	int Verify(void);

public:

	//Установить задержку
	int SetDelay(__u16 delay);

	//Прочитать задержку
	int GetDelay(__u16* data);

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}

};

//inline LIZA_B0608::~LIZA_B0608() { Unbind(); }


#endif	//_LIZA_B0608_h
