
#include <stdio.h>
#include "Coincide_C0308.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>

const char Coincide_C0308::Type[]="Coincide_C0308   ";

Coincide_C0308::Coincide_C0308()
	: Base(Type, &dfcamac_lcm_none)
{
	for(size_t i=0; i<=maxChannel; i++)
		SettingAndMask[i] = badAndMask;
	SettingOrMask = badOrMask;
}

int Coincide_C0308::Init(const u8_t *maskAND,  u8_t maskOR)
{
	int rc = Base::Init();
	for(COI_ch_t i=0; i<=maxChannel; i++)
	{
		rc |= SetMaskAND(maskAND[i], i);
	}
	rc |= SetMaskOR(maskOR);
	return rc;
}

int Coincide_C0308::Init(void)
{
	u8_t maskAND[nChannels], maskOR,  *pm = maskAND;
	int rc = Base::Init();
	for(COI_ch_t i=0; i<=maxChannel; i++)
		rc |= GetMaskAND(pm++, i);
	rc |= GetMaskOR(&maskOR);

	return rc | Init( maskAND, maskOR );
}

dev_test_state Coincide_C0308::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if( dt!=DEV_TEST_OK ) return dt;

	int rc = 0;

	for(COI_ch_t i=0; i<=maxChannel; i++)
	{
		u8_t SavedMask = SettingAndMask[i];
		for(u32_t im=0; im<=maxAndMask; im++)
		{
			u8_t data;
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

	u8_t SavedMask = SettingOrMask;
	for(u32_t im=0; im <= maxOrMask; im++)
	{
		u8_t data;
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

int Coincide_C0308::Verify(void)
{
	int rc = Base::Verify();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	u8_t mask;
	for(COI_ch_t i=0; i<=maxChannel; i++)
	{
		rc = GetMaskAND(&mask, i);
		if(  rc&CAMAC_CC_ERRORS  ) 	return rc;
		if( mask != SettingAndMask[i] )
			return CAMAC_CC_VERIFICATION_ERROR;
	}

	rc = GetMaskOR(&mask);
	if( mask != SettingOrMask )
			return CAMAC_CC_VERIFICATION_ERROR;
	return rc;
}

const camac_af_t  Coincide_C0308::afSetAND[nChannels] =
{	CAMAC_INIT_AF(0,17),	CAMAC_INIT_AF(1,17),
	CAMAC_INIT_AF(2,17),	CAMAC_INIT_AF(3,17)		};

const camac_af_t  Coincide_C0308::afGetAND[nChannels] =
{	CAMAC_INIT_AF(0, 1),	CAMAC_INIT_AF(1, 1),
	CAMAC_INIT_AF(2, 1),	CAMAC_INIT_AF(3, 1)		};

//-------------------------------

int Coincide_C0308::SetMaskOR( u16_t mask)
{
	if (mask & badOrMask) return CAMAC_CC_INVALID_ARG;
	int rc = AF(afSetOR, &mask);
	if ( !rc )
		SettingOrMask = mask;
	return rc;
}

int Coincide_C0308::GetMaskOR(u8_t* mask)
{
	u16_t tmp;
	int rc = AF(afGetOR, &tmp);

	if( tmp & badOrMask )
		*mask = bad( tmp );
	else
		*mask = tmp;

	return rc;
}

int Coincide_C0308::SetMaskAND( u16_t mask, COI_ch_t ch)
{
	if ( mask & badAndMask || ch & badChannel )
		return CAMAC_CC_INVALID_ARG;
	u16_t dummy = mask;
//	u8_t tmp = SettingAndMask[ch];
//	SettingAndMask[ch] = mask;
	int rc = AF(afSetAND[ch], &dummy);
	if ( rc )
		SettingAndMask[ch] = dummy;
	return rc;
}

int Coincide_C0308::GetMaskAND(u8_t* mask, COI_ch_t ch)
{
	u16_t tmp;
	int rc = AF(afGetAND[ch], &tmp);
	*mask = tmp & maxAndMask;
	return rc;
}
