
#include <stdio.h>
#include "DelayPulseShaper_L0323.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char DelayPulseShaper_L0323::Type[] = "DelayPulseShaper_L0323";

DelayPulseShaper_L0323::DelayPulseShaper_L0323()
	: Base(Type, &dfcamac_lcm_none)
{
	for( DPSh_ch_t i=0; i < nChannels; i++ )
		Duration[i] = badDuration;
}

int DelayPulseShaper_L0323::Init(void)
{
	int rc=Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	__u16 data;
	for ( DPSh_ch_t i=0; i<nChannels; i++)
		rc |= GetDuration(i, &data) | SetDuration(i, data);

	return rc;
}

int DelayPulseShaper_L0323::Init(const __u8* Dur)
{
	int rc=Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	for ( DPSh_ch_t i=0; i<nChannels; i++)
		rc |= SetDuration(i, Dur[i]);
	return rc;
}

int DelayPulseShaper_L0323::Init(__u8 Dur, DPSh_ch_t ch)
{
	return Base::Init() | SetDuration(ch, Dur);
}

dev_test_state DelayPulseShaper_L0323::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	int rc = 0;
	for ( DPSh_ch_t ch=0; ch<nChannels; ch++)
	{
		__u8 SavedDuration = Duration[ch];
		__u16 data;
		for(u32_t i=0; i<=maxDuration; i++)
		{
			rc |= SetDuration(ch,i) | GetDuration(ch,&data);
			if (data!=i)
			{
				fprintf( stderr, "%s: Test error: Channel=%d  "
					"Wrote=%2.2x  Read=%2.2x\n"
					,Type, ch, i, data );
				dt = DEV_TEST_BROKEN;
			}
		}
		rc |= SetDuration(ch,SavedDuration);
	}

	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int DelayPulseShaper_L0323::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	for ( DPSh_ch_t ch=0; ch<nChannels; ch++)
	{
		__u16 data;
		rc |= GetDuration(ch,&data);
		if(  rc&CAMAC_CC_ERRORS  ) 	return rc;
        if( data != Duration[ch] ) 	return CAMAC_CC_VERIFICATION_ERROR;
	}

	return rc;
}

//-------------------------------


const camac_af_t DelayPulseShaper_L0323::afRead[nChannels] =
{
	CAMAC_INIT_AF(0, 1),
	CAMAC_INIT_AF(1, 1),
};

const camac_af_t DelayPulseShaper_L0323::afWrite[nChannels] =
{
	CAMAC_INIT_AF(0,17),
	CAMAC_INIT_AF(1,17),
};

int DelayPulseShaper_L0323::SetDuration(DPSh_ch_t channel, __u16 duration)
{
	if (channel & badChannels) return CAMAC_CC_INVALID_ARG;
	int rc = AF(afWrite[channel], &duration);
	if ( !rc )
		Duration[channel] = duration;
    return rc;
}

int DelayPulseShaper_L0323::GetDuration(DPSh_ch_t channel, __u16* data)
{
	if (channel & badChannels) return CAMAC_CC_INVALID_ARG;
	int rc=AF(afRead[channel], data);
	if( *data > maxDuration )
		*data = bad(*data);
	return rc;
}
