/*
$Id: CAS.h,v 1.2 2003/01/15 08:20:24 barnykov Exp $
*/

#ifndef  _CAS_h
# define _CAS_h

#include <unistd.h>
#include <camac/dfmodule/base.h>
#include <camac/df/std_lcm.h>
#include <linux/types.h>

class CAS : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;
public:
        static const char Type[];
	enum Mode { SINGLE=2, DOUBLE=1 };
	enum ChipSet { GERKON=0, MOP=1 };
	static const __u8 notValidChannel=255;
	const __u8 maxChannel; //may be static const member of derived class

private:
	const char* type;
	static const ulong WaitSwitchTime[2];
	__u8 nDWChannels; //Number of doublewires channels, must be static const member of derived class
	ChipSet cs;
	static const int CHANNEL_NOT_IN_RANGE = CAMAC_CC_USER_ERROR0;

protected:
	__u8 Channel;
	const Mode mode;

	enum
	{
		afRead		= CAMAC_INIT_AF(0, 0),
		afSwitch 	= CAMAC_INIT_AF(0, 16)
	};

	CAS(const char* _type, __u8 _nDWChannels, ChipSet _cs, Mode _mode, const camac_lam_check_method* _lcm=&dfcamac_lcm_none);
	virtual ~CAS();

public:
	int Bind(const camac_address& addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
	{ return Base::Bind(addr, lock_station_timeout,
						flags &	~( 	CAMAC_OPEN_STATION_CZ_SENSITIVE
							  |		CAMAC_OPEN_STATION_LOCK_CZ 		) );	}
	inline int Unbind(void)
		{ return Base::Unbind(); 					}

	inline int Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 		}
	inline int Unlock(void)
		{ return Base::UnlockStation(); 			}

	int Init(__u8 ch);
	inline int Init(void)
		{ return Init(0); }

	virtual dev_test_state Test(void);

	int Verify(void);

	virtual int Switch(__u8 ch);

	virtual int Read(__u8* ch);

	inline void Wait(void)
		{ usleep(WaitSwitchTime[cs]); }

//	int PrintErrors(const char* str, int rc);

};

#endif	//_CAS_h

/*
$Log: CAS.h,v $
Revision 1.2  2003/01/15 08:20:24  barnykov
Some changes...

Revision 1.1  2001/05/29 13:33:04  barnykov
BMYuCamacModule* added

*/
