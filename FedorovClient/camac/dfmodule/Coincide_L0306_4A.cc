#include <stdio.h>
#include "Coincide_L0306_4A.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>

const char Coincide_L0306_4A::Type[]="Coincide_L0306_4A   ";

Coincide_L0306_4A::Coincide_L0306_4A()
	: Base(Type, &dfcamac_lcm_none)
{
	for(size_t i=0; i<=maxChannel; i++)
		SetingAndMask[i] = badAndMask;
	SetingOrMask = badOrMask;
}

int Coincide_L0306_4A::Init(const __u8 *maskAND,  __u8 maskOR)
{
	int rc = Base::Init();
	for(COI_ch_t i=0; i<=maxChannel; i++)
		rc |= SetMaskAND(maskAND[i], i);
	rc |= SetMaskOR(maskOR);
	return rc;
}

int Coincide_L0306_4A::Init(void)
{
	__u8 maskAND[nChannels], maskOR,  *pm = maskAND;
	int rc = Base::Init();
	for(COI_ch_t i=0; i<=maxChannel; i++)
		rc |= GetMaskAND(pm++, i);
	rc |= GetMaskOR(&maskOR);

	return rc | Init( maskAND, maskOR );
}

dev_test_state Coincide_L0306_4A::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if( dt!=DEV_TEST_OK ) return dt;

	int rc = 0;

	for(COI_ch_t i=0; i<=maxChannel; i++)
	{
		__u8 SavedMask = SetingAndMask[i];
		for(u16_t im=0; im<=maxAndMask; im++)
		{
			__u8 data;
			rc |= SetMaskAND(im, i) | GetMaskAND(&data, i);
			if ( data!=im )
			{
				fprintf( stderr,
					"%s: Test error: Channel %1d  "
					"Wrote=0x%2.2x  Read=0x%2.2x\n"
					,Type, i, im, data );
				dt = DEV_TEST_BROKEN;
			}
		}
		rc |= SetMaskAND(SavedMask, i);
	}

	__u8 SavedMask = SetingOrMask;
	for(u16_t im=0; im <= maxOrMask; im++)
	{
		__u8 data;
		rc |= SetMaskOR(im) | GetMaskOR(&data);
		if( data!=im )
		{
			fprintf( stderr,
				"%s: Test error: OR Channel  "
				"Wrote=0x%2.2x  Read=0x%2.2x\n"
				,Type, im, data );
			dt = DEV_TEST_BROKEN;
		}
	}
	rc |= SetMaskOR( SavedMask );

	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int Coincide_L0306_4A::Verify(void)
{
	int rc = Base::Verify();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	__u8 mask;
	for(COI_ch_t i=0; i<=maxChannel; i++)
	{
		rc = GetMaskAND(&mask, i);
		if(  rc&CAMAC_CC_ERRORS  ) 	return rc;
		if( mask != SetingAndMask[i]	 )
			return CAMAC_CC_VERIFICATION_ERROR;
	}

	rc = GetMaskOR(&mask);
	if( mask != SetingOrMask )
			return CAMAC_CC_VERIFICATION_ERROR;
	return rc;
}

const camac_af_t  Coincide_L0306_4A::afSetAND[nChannels] =
{	CAMAC_INIT_AF(0,17),	CAMAC_INIT_AF(1,17),
	CAMAC_INIT_AF(2,17),	CAMAC_INIT_AF(3,17)		};

const camac_af_t  Coincide_L0306_4A::afGetAND[nChannels] =
{	CAMAC_INIT_AF(0, 1),	CAMAC_INIT_AF(1, 1),
	CAMAC_INIT_AF(2, 1),	CAMAC_INIT_AF(3, 1)		};

//-------------------------------

int Coincide_L0306_4A::SetMaskOR( __u16 mask)
{
	if (mask & badOrMask) return CAMAC_CC_INVALID_ARG;
	int rc = AF(afSetOR, &mask);
	if ( !rc )
		SetingOrMask = mask;
	return rc;
}

//Прочитать маску
//Возвращает camac_cc_bits.
//bad(*mask) при перегрузке.
int Coincide_L0306_4A::GetMaskOR(__u8* mask)
{
	__u16 tmp;
	int rc = AF(afGetOR, &tmp);

	if( tmp & badOrMask )
		*mask = bad( tmp );
	else
		*mask = tmp;

	return rc;
}

int Coincide_L0306_4A::SetMaskAND( __u16 mask, COI_ch_t ch)
{
	if ( mask & badAndMask || ch & badChannel )
		return CAMAC_CC_INVALID_ARG;
	int rc = AF(afSetAND[ch], &mask);
	if ( !rc )
		SetingAndMask[ch] = mask;
	return rc;
}

int Coincide_L0306_4A::GetMaskAND(__u8* mask, COI_ch_t ch)
{
    __u16 tmp;
	int rc = AF(afGetAND[ch], &tmp);
	*mask = tmp & maxAndMask;
	return rc;
}
