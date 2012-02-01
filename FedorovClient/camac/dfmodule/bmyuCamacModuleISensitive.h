/*
$Id: bmyuCamacModuleISensitive.h,v 1.1 2001/05/29 13:33:05 barnykov Exp $
*/

#ifndef  _bmyuCamacModuleISensitive_h
# define _bmyuCamacModuleISensitive_h

#include "bmyuCamacModuleCommon.h"

class bmyuCamacModuleISensitive	: virtual public bmyuCamacModuleCommon
{
protected:
	inline bmyuCamacModuleISensitive()
	{
		SetOpenFlags(
			0 ,
			CAMAC_OPEN_STATION_CZ_SENSITIVE|CAMAC_OPEN_STATION_LOCK_CZ
			);
	}

public:
	inline int SetI			(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
		{ return bmyuCamacModuleCommon::SetAndLockI  (timeout);}
	inline int ClearI		(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return bmyuCamacModuleCommon::ClearAndLockI(timeout);}
	inline int UnlockI		(void)
		{ return bmyuCamacModuleCommon::UnlockI();				}
};

#endif	//_bmyuCamacModuleISensitive_h

/*
$Log: bmyuCamacModuleISensitive.h,v $
Revision 1.1  2001/05/29 13:33:05  barnykov
BMYuCamacModule* added

*/
