/*
$Id: Attenuator_A0605.cpp,v 1.2 2001/05/29 13:14:01 barnykov Exp $
*/

#include <stdio.h>
#include <unistd.h>
#include "Attenuator_A0605.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>

const char Attenuator_A0605::Type[]="Attenuator_A0605   ";

int Attenuator_A0605::Init( __u8 mask)
{
	SetingMask = notValidMask;
	return Base::Init() | SetAtt(mask);
}

int Attenuator_A0605::Init(void)
{
	__u8 mask;
	return GetAtt(&mask) | Init(mask);
}

dev_test_state Attenuator_A0605::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

    __u8 SavedMask = SetingMask;

	int rc = 0;
	for(__u8 i=0; i <= maxMask; i++)
	{
		__u8 data;
		rc |= SetAtt(i) | GetAtt(&data);
		if ( data!=i )
		{
			fprintf( stderr,
				"%s: Test error:  "
				"Wrote=0x%1.1x  Read=0x%1.1x\n"
				,Type, i, data );
			dt = DEV_TEST_BROKEN;
		}
	}
	rc |= SetAtt(SavedMask);

	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int Attenuator_A0605::Verify(void)
{
	int rc = Base::Verify();
	if( rc ) return rc;

	__u8 mask;
	rc = GetAtt(&mask);
	if( rc&CAMAC_CC_ERRORS ) 	return rc;
	if( mask != SetingMask ) 	return CAMAC_CC_VERIFICATION_ERROR;

	return rc;
}

//-------------------------------

int Attenuator_A0605::SetAtt( __u16 mask)
{
	if (mask > maxMask) return CAMAC_CC_INVALID_ARG;
	int rc = AF(afSet, &mask);
    usleep(WaitAfterSetAtt);
	if ( !rc )
		SetingMask = mask;
	return rc;
}

//Прочитать маску
//Возвращает camac_cc_bits.
//bad(*mask) при перегрузке.
int Attenuator_A0605::GetAtt(__u8* mask)
{
	__u16 t;
	int rc=AF(afGet, &t);
//	if( t > maxMask )
//		*mask = bad( t );
//	else
	*mask = t&maxMask;

	return rc;
}

int Attenuator_A0605::GetAtt(float* db)
{
	__u8 t;
	int rc = GetAtt(&t);
    *db = code2dB(t);
	return rc;
}

/*
$Log: Attenuator_A0605.cpp,v $
Revision 1.2  2001/05/29 13:14:01  barnykov
BMYuCamacModuleCommon added.

Revision 1.1.1.1  2001/03/24 14:55:31  barnykov
Переношу в правильный репозиторий...

Revision 1.1  2001/03/24 02:57:45  barnykov
Иду на Kedr-10.

*/

