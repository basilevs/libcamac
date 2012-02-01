/*
$Id: Discriminator_D0302.cpp,v 1.1.1.1 2001/03/24 14:55:31 barnykov Exp $
*/

#include <stdio.h>
#include "Discriminator_D0302.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>

const char Discriminator_D0302::Type[]="Discriminator_D0302   ";

Discriminator_D0302::Discriminator_D0302()
	: Base(Type, &dfcamac_lcm_none)
{
	for(DISC_ch_t i=0; i<=maxChannel; i++)
		SetingMask[i] = badMask;
}

int Discriminator_D0302::Init(const u16_t *mask)
{
	int rc = Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;
	for(DISC_ch_t i=0; i<=maxChannel; i++)
		rc |= SetThreshold(mask[i], i);
	return rc;
}

int Discriminator_D0302::Init(u16_t mask, DISC_ch_t ch)
{
	return Base::Init() | SetThreshold(mask, ch);
}

int Discriminator_D0302::Init(void)
{
	int rc = Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	u16_t mask[nChannels];
	for(DISC_ch_t i=0; i<=maxChannel; i++)
		rc |= GetThreshold(&mask[i], i);

	return rc | Init( mask );
}

dev_test_state Discriminator_D0302::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if( dt!=DEV_TEST_OK ) return dt;

	int rc = 0;
	for(DISC_ch_t i=0; i<=maxChannel; i++)
	{
		u8_t SavedMask = SetingMask[i];
		for(u32_t im=0; im<=maxMask; im++)
		{
			u16_t data;
			rc |= SetThreshold( im, i) | GetThreshold(&data, i);
			if ( data!=im )
			{
				fprintf( stderr, "%s: Test error: Channel=%d  "
					"Wrote=%2.2x  Read=%2.2x\n"
					,Type, i, im, data );
				dt = DEV_TEST_BROKEN;
			}
		}
		rc |= SetThreshold(SavedMask, i);
	}
	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int Discriminator_D0302::Verify(void)
{
	int rc = Base::Verify();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	u16_t mask;
	for(DISC_ch_t i=0; i<=maxChannel; i++)
	{
		rc = GetThreshold(&mask, i);
		if(  rc&CAMAC_CC_ERRORS  ) 	return rc;
		if( mask != SetingMask[i] )
			return CAMAC_CC_VERIFICATION_ERROR;
	}

	return rc;
}

const camac_af_t  Discriminator_D0302::afSet[nChannels] =
{	CAMAC_INIT_AF(0,17),	CAMAC_INIT_AF(1,17),
	CAMAC_INIT_AF(2,17),	CAMAC_INIT_AF(3,17),	};

const camac_af_t  Discriminator_D0302::afGet[nChannels] =
{	CAMAC_INIT_AF(0, 1),	CAMAC_INIT_AF(1, 1),
	CAMAC_INIT_AF(2, 1),	CAMAC_INIT_AF(3, 1),	};

//-------------------------------

int Discriminator_D0302::SetThreshold(u16_t mask, DISC_ch_t ch)
{
	if ( ch & badChannel ) return CAMAC_CC_INVALID_ARG;
	int rc = AF(afSet[ch], &mask);
	if ( !rc )
		SetingMask[ch] = mask;
	return rc;
}

int Discriminator_D0302::GetThreshold(u16_t* mask, DISC_ch_t ch)
{
	int rc = AF(afGet[ch], mask);
	*mask &= maxMask;
	return rc;
}

int Discriminator_D0302::GetThreshold(float*  UmV, DISC_ch_t ch)
{
    u16_t tmp;
	int rc = AF(afGet[ch], &tmp);
	*UmV = code2UmV( tmp&maxMask );
	return rc;
}

