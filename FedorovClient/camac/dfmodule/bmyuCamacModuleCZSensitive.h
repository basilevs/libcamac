/*
$Id: bmyuCamacModuleCZSensitive.h,v 1.1 2001/05/29 13:33:05 barnykov Exp $
*/

#ifndef  _bmyuCamacModuleCZSensitive_h
# define _bmyuCamacModuleCZSensitive_h

#include "bmyuCamacModuleCommon.h"

class bmyuCamacModuleCZSensitive : virtual public bmyuCamacModuleCommon
{
protected:
	inline bmyuCamacModuleCZSensitive()
	{
		SetOpenFlags(
			CAMAC_OPEN_STATION_CZ_SENSITIVE|CAMAC_OPEN_STATION_LOCK_CZ
			, 0
			);
	}

public:
	int CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);

	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
		{ return Lock  (CAMAC_LOCK_CZ, timeout);		}
	inline int UnLockCZ(void)
		{ return Unlock(CAMAC_LOCK_CZ);					}

};

#endif	//_bmyuCamacModuleCZSensitive_h

/*
$Log: bmyuCamacModuleCZSensitive.h,v $
Revision 1.1  2001/05/29 13:33:05  barnykov
BMYuCamacModule* added

*/
