
#include <stdio.h>
#include "IntegralVoltmeter_C0309.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>
#include <limits.h>

const char IV_C0309::Type[]="IV_C0309   ";

//-------------------------------

IV_C0309::IV_C0309()
	: Base(Type, &dfcamac_lcm_none)
{
	Control = 0;
}

int IV_C0309::Init(void)
{
	int rc=Base::Init();
	if( rc ) return rc;
	__u16 dummy_control;
    rc = AF(afClearLAM);
	if( (rc|=AF(afControlRead, &dummy_control))&CAMAC_CC_ERRORS ) return rc;
	Control = dummy_control & ControlMask;
	return rc;
}

int IV_C0309::Init(const bool Scale, const __u8 Time)
{
	int rc=Base::Init();
	rc |= ClearLAM();
	rc |= SetControl(Scale, Time);
	return rc;
}

dev_test_state IV_C0309::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
    int rc=AF(afClearLAM);
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	if( (dt=dfcamac_rc2test(SetControl(0,0)|AF(afStart)))!=DEV_TEST_OK )
		return dt;

	if( (dt=dfcamac_rc2test(SetControl(0,0)|AF(afStop)))!=DEV_TEST_OK )
		return dt;

	rc = AF(afCheckLAM);
	if ( rc & (CAMAC_CC_ERRORS & ~CAMAC_CC_NOT_Q) )
		return dfcamac_rc2test(rc & ~CAMAC_CC_NOT_Q);
	if ( !(rc & CAMAC_CC_NOT_Q) )
		return DEV_TEST_BROKEN;

	return DEV_TEST_OK;
}

int IV_C0309::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	__u16 dummy_control;
		rc = AF(afControlRead,&dummy_control);
		if( rc&CAMAC_CC_ERRORS )
			return rc;
		if( dummy_control != Control )
			return CAMAC_CC_VERIFICATION_ERROR;

	return 0;
}


int IV_C0309::SetControl(const bool Scale, const __u8 Time)
{
	__u16 data = ((Scale&ScaleMask)<<3) | (Time&TimeMask);
	int rc = AF(afControlWrite, &data);
	if( !rc )
		Control = ControlMask&data;
	if (rc & CAMAC_CC_ERRORS ) {
		return rc;
	}
#ifndef NDEBUG
	bool Scale1 = 0;
	__u8 Time1  = 0;
	rc |= ReadControl(&Scale1, &Time1);
	if (rc & CAMAC_CC_ERRORS ) return rc;
	if (Scale1 != Scale || Time1 != Time)
		return 1;
#endif
    return rc;
}

int IV_C0309::ReadControl(bool* Scale, __u8* Time)
{
	__u16 data;
	int rc=AF(afControlRead, &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	*Time = data&TimeMask;
	*Scale= (data>>3)&ScaleMask;
	return rc;
}

#include <iostream>
#include <iomanip>

int IV_C0309::ReadVoltage(int* v)
{
	__u32 code;
	*v = USHRT_MAX;
	int rc=AF(afRead,&code);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
	bool D = (Control>>3)&ScaleMask;
	__u8 T = Control&TimeMask;
//	clog <<  "D=" << D << ", T=" << (int) T << ", code=" << code 
//		 << ", codeint=" << static_cast<int>(code/(1<<T)) << dec << ", 9th=" << 022000 <<endl;
	*v =static_cast<int>(( 022000 - (code/(1<<T)) ) / (1+15*D));
//	clog << "voltage = " << *v << endl;
//	*v=static_cast<__u16>(code);
	return rc;
}
