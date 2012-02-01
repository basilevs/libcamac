/*
$Id: bmyuCamacModuleNotSensitive.h,v 1.1 2001/05/29 13:33:05 barnykov Exp $
*/

#ifndef  _bmyuCamacModuleNotSensitive_h
# define _bmyuCamacModuleNotSensitive_h

#include "bmyuCamacModuleCommon.h"

class bmyuCamacModuleNotSensitive	: virtual public bmyuCamacModuleCommon
{
protected:
	inline bmyuCamacModuleNotSensitive()
	{
		SetOpenFlags(
			0 ,
			CAMAC_OPEN_STATION_CZ_SENSITIVE|CAMAC_OPEN_STATION_LOCK_CZ
			);
	}
};

#endif	//_bmyuCamacModuleNotSensitive_h

/*
$Log: bmyuCamacModuleNotSensitive.h,v $
Revision 1.1  2001/05/29 13:33:05  barnykov
BMYuCamacModule* added

*/
