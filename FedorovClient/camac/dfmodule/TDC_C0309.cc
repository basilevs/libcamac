#include <stdio.h>
#include "TDC_C0309.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>

const char TDC_C0309::Type[]="TDC_C0309";

TDC_C0309::TDC_C0309()
	: Base(Type, &dfcamac_lcm_a0_f8_q)
{
}

int TDC_C0309::Init(void)
{
	return Base::Init() | Reset();
}

dev_test_state TDC_C0309::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	int rc = Reset();
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	return DEV_TEST_OK;
}

//-------------------------------

const camac_af_t TDC_C0309::afReadClearLAM[nChannels] =
{
	CAMAC_INIT_AF(0, 2),
	CAMAC_INIT_AF(1, 2),
	CAMAC_INIT_AF(2, 2),
	CAMAC_INIT_AF(3, 2)
};

const camac_af_t TDC_C0309::afRead[nChannels] =
{
	CAMAC_INIT_AF(0, 0),
	CAMAC_INIT_AF(1, 0),
	CAMAC_INIT_AF(2, 0),
	CAMAC_INIT_AF(3, 0)
};

struct camac_af_list TDC_C0309::read_all[] =
{
	{ CAMAC_INIT_AF16(0, 0), 0 },
	{ CAMAC_INIT_AF16(1, 0), 0 },
	{ CAMAC_INIT_AF16(2, 0), 0 },
	{ CAMAC_INIT_AF16(3, 0), 0 }
};
struct camac_af_list TDC_C0309::read_all_reset_lam[] =
{
	{ CAMAC_INIT_AF16(0, 2), 0 },
	{ CAMAC_INIT_AF16(1, 2), 0 },
	{ CAMAC_INIT_AF16(2, 2), 0 },
	{ CAMAC_INIT_AF16(3, 2), 0 }//,
//	{ CAMAC_INIT_AF  (0,10)	   }
};

int TDC_C0309::CZReset(df_timeout_t* timeout)
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

int TDC_C0309::read(const camac_af_t *af, TDC_ch_t channel, __u16* data)
{
	if ( channel & badChann ) return CAMAC_CC_INVALID_ARG;
	int rc = AF( af[channel], data );
	*data = checkdata(*data);
	return rc;
}

int TDC_C0309::read( camac_af_list *aflist, size_t aflist_size, __u16* data)
{
	int rc = ListAF( aflist, aflist_size);
	for(TDC_ch_t i=0; i<nChannels; i++)
		data[i] = checkdata( (aflist++)->data );

	return rc;
}
