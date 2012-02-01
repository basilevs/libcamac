
#include "GTP_B0609.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>
#include <dfcommonxx/bad.h>

const char GTP_B0609::Type[]="GTP_B0609    ";

GTP_B0609::GTP_B0609()
	: Base(Type, &dfcamac_lcm_none)
{
}

int GTP_B0609::Init(void)
{
	int rc =  Base::Init();
	if (rc) return rc;
	
	rc |= this->Reset();

	return rc;
}

dev_test_state GTP_B0609::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	__u16 data=100;
	int rc = 0 | this->Set(data) | this->Reset();
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	return DEV_TEST_OK;
}

int GTP_B0609::CZReset(df_timeout_t* timeout)
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

int GTP_B0609::Reset(void)
{
	return this->Set(18);		//equals zero after convertion, actually
}
