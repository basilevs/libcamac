/*
$Id: bmyuCamacModuleCZSensitive.cpp,v 1.1 2001/05/29 13:33:05 barnykov Exp $
*/

#include "bmyuCamacModuleCZSensitive.h"

int bmyuCamacModuleCZSensitive::CZReset(df_timeout_t* timeout)
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

/*
$Log: bmyuCamacModuleCZSensitive.cpp,v $
Revision 1.1  2001/05/29 13:33:05  barnykov
BMYuCamacModule* added

*/
