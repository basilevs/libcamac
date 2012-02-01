/*
$Id: CAS_16G.h,v 1.2 2003/01/15 08:20:24 barnykov Exp $
*/

#ifndef  _CAS_16G_h
# define _CAS_16G_h

#include "CAS.h"
#include <unistd.h>
#include <camac/dfmodule/base.h>
#include <camac/df/std_lcm.h>

class CAS_16G : public CAS
{
public:
	static const __u8 nDWChannels=16;

private:
	static const ChipSet cs=GERKON;

public:
	inline  CAS_16G(Mode _mode) : CAS(Type, nDWChannels, cs, _mode)
	{/* printf("cahnnel=%d\n",Channel);*/ }
	inline ~CAS_16G() { Unbind(); }

};

#endif	//_CAS_16G_h

/*
$Log: CAS_16G.h,v $
Revision 1.2  2003/01/15 08:20:24  barnykov
Some changes...

Revision 1.1  2001/05/29 13:33:04  barnykov
BMYuCamacModule* added

*/
