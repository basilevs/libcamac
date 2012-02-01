/*
$Id: bmyuCamacModuleLAMable.h,v 1.1 2001/05/29 13:33:05 barnykov Exp $
*/

#ifndef  _bmyuCamacModuleLAMable_h
# define _bmyuCamacModuleLAMable_h

#include <camac/df/wait_lam.h>
#include "bmyuCamacModuleCommon.h"

class bmyuCamacModuleLAMable
	: virtual public bmyuCamacModuleCommon
{
//	const struct camac_lam_check_method* lcm;
//	const camac_af_t afLAMdisable, afLAMenable;

protected:
	inline bmyuCamacModuleLAMable()
	{
		SetOpenFlags(
			0 ,
			CAMAC_OPEN_STATION_CZ_SENSITIVE|CAMAC_OPEN_STATION_LOCK_CZ
			);
	}

public:
	inline int WaitLAM(df_timeout_t* timeout)
		{ return dfcamac_wait_lam(GetFD(), timeout);		}

};

#endif	//_bmyuCamacModuleLAMable_h

/*
$Log: bmyuCamacModuleLAMable.h,v $
Revision 1.1  2001/05/29 13:33:05  barnykov
BMYuCamacModule* added

*/
