/*
$Id: CAS_128G.h,v 1.1 2003/01/15 08:20:24 barnykov Exp $
*/

#ifndef  _CAS_128G_h
# define _CAS_128G_h

#include "CAS.h"
#include <unistd.h>
#include <camac/dfmodule/base.h>
#include <camac/df/std_lcm.h>

class CAS_128G : public CAS
{
public:
	static const __u8 nDWChannels=128;

private:
	static const ChipSet cs=GERKON;

public:
	inline  CAS_128G(Mode _mode) : CAS(Type, nDWChannels, cs, _mode)
	{/* printf("cahnnel=%d\n",Channel);*/ }
	inline ~CAS_128G() { Unbind(); }

};


#endif	//_CAS_128G_h

/*

*/
