
#include "TDC_C0301.h"
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>

const char TDC_C0301::Type[]="TDC_C0301    ";

TDC_C0301::TDC_C0301()
	: Base(Type, &dfcamac_lcm_none)
{
}

int TDC_C0301::Init(void)
{
	int rc =  Base::Init();
	if (rc)
		return rc;

	rc |= Reset();

	return rc;
}

dev_test_state TDC_C0301::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	__u16 data;
	int rc = 0;
	for ( TDC_ch_t i=0; i<nCh; i++)
		rc |= ReadClear(i, &data);
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	return DEV_TEST_OK;
}

const camac_af_t TDC_C0301::afRead[nCh] =
{
	CAMAC_INIT_AF(0, 0), CAMAC_INIT_AF(1, 0),
	CAMAC_INIT_AF(2, 0), CAMAC_INIT_AF(3, 0)
};

const camac_af_t TDC_C0301::afReadClear[nCh] =
{
	CAMAC_INIT_AF(0, 2), CAMAC_INIT_AF(1, 2),
	CAMAC_INIT_AF(2, 2), CAMAC_INIT_AF(3, 2)
};

// const camac_af_t TDC_C0301::afCheckLAM[nCh] =
// {
	// CAMAC_INIT_AF(0, 8), CAMAC_INIT_AF(1, 8),
	// CAMAC_INIT_AF(2, 8), CAMAC_INIT_AF(3, 8)
// };

// const camac_af_t TDC_C0301::afClear[nCh] =
// {
	// CAMAC_INIT_AF(0, 10), CAMAC_INIT_AF(1, 10),
	// CAMAC_INIT_AF(2, 10), CAMAC_INIT_AF(3, 10)
// };

int TDC_C0301::CZReset(df_timeout_t* timeout)	//doesn't work
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

int TDC_C0301::Reset(void)
{
	return (AF(afClear) & ~CAMAC_CC_NOT_Q)
		| CompleteLAM();
}
