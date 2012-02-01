
#include <stdio.h>
#include "HVS_B0308.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char HVS_B0308::Type[]="HVS_B0308   ";

const camac_af_t HVS_B0308::afRead[nChannels] =
{
	CAMAC_INIT_AF(0, 0),
	CAMAC_INIT_AF(1, 0)
};

const camac_af_t HVS_B0308::afWrite[nChannels] =
{
	CAMAC_INIT_AF(0,16),
	CAMAC_INIT_AF(1,16)
};

const camac_af_t HVS_B0308::afOn[nChannels] =
{
	CAMAC_INIT_AF(0,26),
	CAMAC_INIT_AF(1,26)
};

const camac_af_t HVS_B0308::afOff[nChannels] =
{
	CAMAC_INIT_AF(0,24),
	CAMAC_INIT_AF(1,24)
};

//-------------------------------

HVS_B0308::HVS_B0308()
	: Base(Type, &dfcamac_lcm_none)
{
	for( size_t i=0; i < nChannels; i++ )
		Code[i] = 0;
}

int HVS_B0308::Init(void)
{
	int rc=Base::Init();
	if( rc ) return rc;
	__u16 code[2];
    rc = AF(afClearLAM);
	for ( size_t i=0; i<nChannels; i++) {
		if( (rc|=AF(afRead[i], &code[i]))&CAMAC_CC_ERRORS ) return rc;
		Code[i] = code[i];
	}
	return rc;
}

int HVS_B0308::Init(__u16* v)
{
	int rc=Base::Init();
	if( rc ) return rc;
	rc = AF(afClearLAM);
	for ( size_t i=0; i<nChannels; i++)
		rc |= AF(afWrite[i],&v[i])|AF(afOn[i]);
	return rc;
}

dev_test_state HVS_B0308::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
    int rc=AF(afClearLAM);
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	for ( size_t i=0; i<nChannels; i++ )
	{
		if( (dt=dfcamac_rc2test(SetVoltage(i,0)|AF(afOff[i])))!=DEV_TEST_OK )
			return dt;

		bool on;

		if( (dt=dfcamac_rc2test(AF(afOn[i])|ReadState(i,&on)))!=DEV_TEST_OK )
			return dt;
		if ( !on )
		{
			printf("%s: Test error: Ch=%d unable to turn on \n"
				, Type, i );
			return DEV_TEST_BROKEN;
		}

		if( (dt=dfcamac_rc2test(AF(afOff[i])|ReadState(i,&on)))!=DEV_TEST_OK )
			return dt;
		if ( on )
		{
			printf("%s: Test error: Ch=%d unable to turn off \n"
				, Type, i );
			return DEV_TEST_BROKEN;
		}

		for(__u8 shift=0; shift<nVoltageBits; shift++)
		{
			__u16 w, r;
			w = 1<<shift;
			if( (dt=dfcamac_rc2test(AF(afWrite[i],&w)))!=DEV_TEST_OK )
				return dt;
			if( (dt=dfcamac_rc2test(ReadVoltage(i,&r)))!=DEV_TEST_OK )
				return dt;
			if (w!=r)
			{
				printf("%s: Test error: Ch=%2d  W=%2d  R=%2d\n"
					, Type, i, w, r);
				return DEV_TEST_BROKEN;
			}
		}
		if( (dt=dfcamac_rc2test(SetVoltage(i,0)|AF(afOn[i])))!=DEV_TEST_OK )
			return dt;

	}
	rc = AF(afCheckLAM);
	if ( rc & (CAMAC_CC_ERRORS & ~CAMAC_CC_NOT_Q) )
		return dfcamac_rc2test(rc & ~CAMAC_CC_NOT_Q);
	if ( !(rc & CAMAC_CC_NOT_Q) )
		return DEV_TEST_BROKEN;

	return DEV_TEST_OK;
}

int HVS_B0308::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	for ( size_t i=0; i<nChannels; i++)
	{
		__u16 code;
		rc = AF(afRead[i],&code);
		if( rc&CAMAC_CC_ERRORS )
			return rc;
		if( code != Code[i] )
			return CAMAC_CC_VERIFICATION_ERROR;
	}

	return 0;
}


int HVS_B0308::Turn(size_t channel, const bool on)
{
	if ( channel >= nChannels ) return CAMAC_CC_INVALID_ARG;
	int rc;
    if ( on==ON )
		rc = AF(afOn[channel]);
	else
		rc = AF(afOff[channel]);
	if ( !rc )
	{
		Code[channel] &= ~OnMask;
		Code[channel] |= on2mask(on);
	}
    return rc;
}

int HVS_B0308::SetVoltage(size_t channel, const __u16 v)
{
	if ( channel >= nChannels ) return CAMAC_CC_INVALID_ARG;
	u16_t data = v;
	int rc = AF(afWrite[channel], &data);
	if( !rc )
		Code[channel] &= ~VoltageMask|v;
    return rc;
}

int HVS_B0308::ResetOverload(size_t channel)
{
	if ( channel >= nChannels ) return CAMAC_CC_INVALID_ARG;
	return AF(afClearLAM);
}

int HVS_B0308::ReadCode(size_t channel, __u16* pcode)
{
	if (channel >= nChannels) return CAMAC_CC_INVALID_ARG;
	u16_t data;
	int rc=AF(afRead[channel], &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*pcode = data;
	return rc;
}

int HVS_B0308::Update(size_t channel)
{
	if (channel >= nChannels) return CAMAC_CC_INVALID_ARG;
	u16_t data;
	int rc = AF(afRead[channel], &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	Code[channel] = data;
	return rc;
}

int HVS_B0308::Update(void)
{
	int rc=0;
	for(size_t i=0; i<nChannels; i++)
		rc |= Update(i);
	return rc;
}

int HVS_B0308::ReadVoltage(size_t channel, __u16* v)
{
	__u16 code;
	int rc=ReadCode(channel,&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*v = Voltage(code);
	return rc;
}

int HVS_B0308::ReadState(size_t channel, bool* s, bool* over)
{
	__u16 code;
	int rc=ReadCode(channel,&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*s = On(code);
	if ( over ) *over = Overload(code);
	return rc;
}

int HVS_B0308::ReadOverload(size_t channel, bool* over)
{
	__u16 code;
	int rc=ReadCode(channel,&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*over = Overload(code);
	return rc;
}

int HVS_B0308::ReadOverloadValue(size_t channel, int &overload_code)
{
	__u16 code;
	int rc=ReadCode(channel,&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	overload_code=(code&OverloadMask)>>12;
	return rc;
}
