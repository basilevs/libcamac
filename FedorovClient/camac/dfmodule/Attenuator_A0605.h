/*
$Id: Attenuator_A0605.h,v 1.3 2003/01/15 08:20:24 barnykov Exp $
*/

#ifndef  _Attenuator_A0605_h
# define _Attenuator_A0605_h

#include <linux/types.h>
//#include <dfcommonxx/bad.h>
#include "bmyuCamacModuleNotSensitive.h"

class Attenuator_A0605 : public bmyuCamacModuleNotSensitive
{

public:
	static const char Type[];

private:
	enum
	{
		maxMask = 0xF, notValidMask = ~maxMask,
		maxdB = 45, WaitAfterSetAtt = 1000 // mksec
	};
	__u8 SetingMask;
	enum
	{
		afSet		= CAMAC_MAKE_AF(0,16),
		afGet		= CAMAC_MAKE_AF(0, 0),
	};

public:

	inline Attenuator_A0605()
		: Base(Type), SetingMask(notValidMask)
	{ };

	int Init( __u8 mask);
	int Init(void);
	dev_test_state Test(void);
	int Verify(void);

private:
	inline  __u8 dB2code( float  db)
		{ return (db > maxdB) ? (__u8)maxMask : (__u8)(db/3 + 0.5); }
	inline float code2dB( __u16 code)
		{ return (code > maxMask) ? (code&maxMask)*3. : code*3.; }

public:

	int SetAtt( __u16 mask);
//	inline int SetAtt( float db) { return SetAtt( dB2code(db) ); }

	int GetAtt(__u8* mask);
	int GetAtt(float*   db);

public:
};

#endif	//_Attenuator_A0605_h
