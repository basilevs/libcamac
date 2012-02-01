
#include <stdio.h>
#include "HVS_B0307.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char HVS_B0307::Type[]="HVS_B0307   ";

const camac_af_t HVS_B0307::afRead = CAMAC_INIT_AF(0, 0);

const camac_af_t HVS_B0307::afWrite= CAMAC_INIT_AF(0,16);

const camac_af_t HVS_B0307::afOn   = CAMAC_INIT_AF(0,26);

const camac_af_t HVS_B0307::afOff  = CAMAC_INIT_AF(0,24);

//-------------------------------

HVS_B0307::HVS_B0307()
	: Base(Type, &dfcamac_lcm_none)
{
	Code = 0;
}

int HVS_B0307::Init(void)
{
	int rc=Base::Init();
	if( rc ) return rc;
	__u16 code;
    rc = AF(afClearLAM)
	   | AF(afRead, &code);
	return rc;
}

int HVS_B0307::Init(__u16 v)
{
	int rc=Base::Init();
	if( rc ) return rc;
	rc = AF(afClearLAM)
	   | AF(afWrite,&v)|AF(afOn);
	return rc;
}

dev_test_state HVS_B0307::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
    int rc=AF(afClearLAM);
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	if( (dt=dfcamac_rc2test(SetVoltage(0)|AF(afOff)))!=DEV_TEST_OK ) return dt;

	bool on;
	if( (dt=dfcamac_rc2test(AF(afOn)|ReadState(&on)))!=DEV_TEST_OK ) return dt;
	if ( !on )
	{
		printf("%s: Test error: Channel 0 unable to turn on \n", Type);
		return DEV_TEST_BROKEN;
	}

	if( (dt=dfcamac_rc2test(AF(afOff)|ReadState(&on)))!=DEV_TEST_OK ) return dt;
	if ( on )
	{
		printf("%s: Test error: Channel 0 unable to turn off \n", Type);
		return DEV_TEST_BROKEN;
	}

	for(__u8 shift=0; shift<nVoltageBits; shift++)
	{
		__u16 w, r;
		w = 1<<shift;
		if( (dt=dfcamac_rc2test(AF(afWrite,&w)))!=DEV_TEST_OK )	return dt;
		if( (dt=dfcamac_rc2test(ReadVoltage(&r)))!=DEV_TEST_OK) return dt;
		if (w!=r)
		{
			printf("%s: Test error: Channel 0  W=%2d  R=%2d\n", Type, w, r);
			return DEV_TEST_BROKEN;
		}
	}
	if( (dt=dfcamac_rc2test(SetVoltage(0)|AF(afOn)))!=DEV_TEST_OK )	return dt;

	rc = AF(afCheckLAM);
	if ( rc & (CAMAC_CC_ERRORS & ~CAMAC_CC_NOT_Q) )
		return dfcamac_rc2test(rc & ~CAMAC_CC_NOT_Q);
	if ( !(rc & CAMAC_CC_NOT_Q) )
		return DEV_TEST_BROKEN;

	return DEV_TEST_OK;
}

int HVS_B0307::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	__u16 code;
	rc = AF(afRead,&code);
	if( rc&CAMAC_CC_ERRORS ) return rc;
	if( code != Code ) return CAMAC_CC_VERIFICATION_ERROR;

	return 0;
}


int HVS_B0307::Turn(const bool on)
{
	int rc;
    if ( on==ON )
		rc = AF(afOn);
	else
		rc = AF(afOff);
	if ( !rc )
	{
		Code &= ~OnMask;
		Code |= on2mask(on);
	}
    return rc;
}

int HVS_B0307::SetVoltage(const __u16 v)
{
	u16_t data = v;
	int rc = AF(afWrite, &data);
	if( !rc ) Code &= ~VoltageMask|v;
    return rc;
}

int HVS_B0307::ResetOverload(void)
{
	return AF(afClearLAM);
}

int HVS_B0307::ReadCode(__u16* pcode)
{
	u16_t data;
	int rc=AF(afRead, &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*pcode = data;
	return rc;
}

int HVS_B0307::Update(void)
{
	u16_t data;
	int rc = AF(afRead, &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	Code = data;
	return rc;
}

int HVS_B0307::ReadVoltage(__u16* v)
{
	__u16 code;
	int rc=ReadCode(&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*v = Voltage(code);
	return rc;
}

int HVS_B0307::ReadState(bool* s, bool* over)
{
	__u16 code;
	int rc=ReadCode(&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*s = On(code);
	if ( over ) *over = Overload(code);
	return rc;
}

int HVS_B0307::ReadOverload(bool* over)
{
	__u16 code;
	int rc=ReadCode(&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*over = Overload(code);
	return rc;
}

int HVS_B0307::ReadOverloadValue(int &overload_code)
{
	__u16 code;
	int rc=ReadCode(&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	overload_code=(code&OverloadMask)>>12;
	return rc;
}
