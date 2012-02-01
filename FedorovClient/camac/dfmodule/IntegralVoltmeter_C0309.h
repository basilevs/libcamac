
#ifndef  _IV_C0309_h
# define _IV_C0309_h

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <dfcommon/bool.h>

class IV_C0309 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

public:
	static const char Type[];
	enum { nCodeBits = 20, CodeMask=0xfffff, ControlMask=0xf,
			ScaleMask=0x1, TimeMask=0x7};

private:
	__u8 Control;		//control word
	enum {
		afRead			= CAMAC_MAKE_AF(0, 0),
		afControlRead	= CAMAC_MAKE_AF(0, 2),
		afControlWrite	= CAMAC_MAKE_AF(0,16),
		afStart			= CAMAC_MAKE_AF(0,26),
		afStop			= CAMAC_MAKE_AF(0,24),
		
		afCheckLAM = CAMAC_MAKE_AF(0, 8),
		afClearLAM = CAMAC_MAKE_AF(0,10)
	};

public:

			IV_C0309();
	inline ~IV_C0309();

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

	int Init(void);
    int Init(const bool Scale, const __u8 Time);
	dev_test_state Test(void);
	int Verify(void);

public:
	int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); }
	int GetEvents    (void) const
		{ return Base::GetEvents(); }


public:
	inline int Start(void) { return AF(afStart); }
	inline int Stop(void)  { return ( AF(afStop) | AF(afClearLAM) ); }
	inline int CheckLAM(void) {return AF(afCheckLAM);}
	inline int ClearLAM(void) {return AF(afClearLAM);}

	int SetControl(const bool Scale, const __u8 Time);
	int ReadControl(bool* Scale, __u8* Time);

	int ReadVoltage(int* v);
};

inline IV_C0309::~IV_C0309() { Unbind();}

#endif


