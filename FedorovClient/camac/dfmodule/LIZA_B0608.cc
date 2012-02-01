#include <stdio.h>
#include "LIZA_B0608.h"
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char LIZA_B0608::Type[] = "LIZA_B0608";
const camac_af_t LIZA_B0608::afRead  = CAMAC_INIT_AF(0, 0);
const camac_af_t LIZA_B0608::afWrite = CAMAC_INIT_AF(0,16);

int LIZA_B0608::Init(void)
{
	int rc = Base::Init();
	if( rc&CAMAC_CC_ERRORS )
		return rc;
	rc |= GetDelay(&setDelay);
	return rc;
}

int LIZA_B0608::Init(__u16 delay)
{
	int rc = Init();
	if( rc&CAMAC_CC_ERRORS )
		return rc;
	rc |= SetDelay(delay);
	return rc;
}

dev_test_state LIZA_B0608::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK)
		return dt;

	__u16 saved_d;
	int rc = GetDelay(&saved_d);
	for ( __u16 wrote_d=minDelay; wrote_d<=maxDelay; wrote_d+=stepDelay )
	{
		__u16 read_d;
		rc |= SetDelay(wrote_d) | GetDelay(&read_d);
		if( read_d != wrote_d )
		{
			fprintf( stderr, "%s: Test error: Wrote=0x%2.2x Read=0x%2.2x\n"
					,Type, wrote_d, read_d );
			dt = DEV_TEST_BROKEN;
		}
		rc |= SetDelay(saved_d);
	}

	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int LIZA_B0608::Verify(void)
{
	int rc = Base::Verify();
	if( rc&CAMAC_CC_ERRORS )
		return rc;

	__u16 read_d;
	rc |= GetDelay(&read_d);
	if( rc&CAMAC_CC_ERRORS )
		return rc;
	if( read_d != setDelay )
		return CAMAC_CC_VERIFICATION_ERROR;

	return rc;
}

//-------------------------------
// Блок не отдает Q

int LIZA_B0608::SetDelay(__u16 delay)
{
	int rc = AF(afWrite, &delay) & ~CAMAC_CC_NOT_Q;
	if ( !rc&CAMAC_CC_ERRORS )
		setDelay = delay;
    return rc;
}

int LIZA_B0608::GetDelay(__u16* delay)
{
	int rc = AF(afRead, delay) & ~CAMAC_CC_NOT_Q;
	if( *delay > maxDelay )
		*delay = bad(*delay);
	return rc;
}
