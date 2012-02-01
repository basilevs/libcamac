#include <stdio.h>
#include "ZIF_L0323.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char ZIF_L0323::Type[] = "ZIF_L0323";

ZIF_L0323::ZIF_L0323()
	: Base(Type, &dfcamac_lcm_none)
{
	for( ZIF_ch_t i=0; i < nChannels; i++ )
		Duration[i] = badDuration;
}

int ZIF_L0323::Init(void)
{
	int rc=Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	__u16 data;
	for ( ZIF_ch_t i=0; i<nChannels; i++)
		rc |= GetDuration(i, &data) | SetDuration(i, data);

	return rc;
}

int ZIF_L0323::Init(const __u8* Dur)
{
	int rc=Base::Init();
	if( rc&CAMAC_CC_ERRORS ) return rc;

	for ( ZIF_ch_t i=0; i<nChannels; i++)
		rc |= SetDuration(i, Dur[i]);
	return rc;
}

int ZIF_L0323::Init(__u8 Dur, ZIF_ch_t ch)
{
	return Base::Init() | SetDuration(ch, Dur);
}

dev_test_state ZIF_L0323::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	int rc = 0;
	for ( ZIF_ch_t ch=0; ch<nChannels; ch++)
	{
		__u8 SavedDuration = Duration[ch];
		__u16 data;
		for(unsigned i=0; i<=maxDuration; i++)
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

int ZIF_L0323::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	for ( ZIF_ch_t ch=0; ch<nChannels; ch++)
	{
		__u16 data;
		rc |= GetDuration(ch,&data);
		if(  rc&CAMAC_CC_ERRORS  ) 	return rc;
        if( data != Duration[ch] ) 	return CAMAC_CC_VERIFICATION_ERROR;
	}

	return rc;
}

//-------------------------------


const camac_af_t ZIF_L0323::afRead[nChannels] =
{
	CAMAC_INIT_AF(0, 1),
	CAMAC_INIT_AF(1, 1),
};

const camac_af_t ZIF_L0323::afWrite[nChannels] =
{
	CAMAC_INIT_AF(0,17),
	CAMAC_INIT_AF(1,17),
};

	//Установить длительность импульса
	//Возвращает camac_сс_bits.
int ZIF_L0323::SetDuration(ZIF_ch_t channel, __u16 duration)
{
	if ( channel & badChannels || duration & ~DurationMask )
		return CAMAC_CC_INVALID_ARG;
	int rc = AF(afWrite[channel], &duration);
	if ( !rc )
		Duration[channel] = duration;
    return rc;
}

	//Прочитать длительность
	//Возвращает camac_cc_bits, CAMAC_RC_NO_Q при неготовности.
	//bad(*data) при перегрузке.
int ZIF_L0323::GetDuration(ZIF_ch_t channel, __u16* data)
{
	if (channel & badChannels) return CAMAC_CC_INVALID_ARG;
	int rc=AF(afRead[channel], data);
	if( *data & ~DurationMask )
		*data = bad(*data);
	return rc;
}


int ZIF_L0323::SetScaler(ZIF_ch_t channel, __u16  scale)
{
	if ( channel & badChannels || scale & ~ScalerMask )
		return CAMAC_CC_INVALID_ARG;
	__u16 data = Scaler & ~( ScalerMask << (ScalerLength*channel) )
		| 					 scale      << (ScalerLength*channel) ;
	int rc = AF(afScalerWrite, &data);
	printf("I am SetScaler(%d,%d), data = %d\n", channel, scale, data);
	if ( !rc )
		Scaler = data;
    return rc;
}

int ZIF_L0323::GetScaler(ZIF_ch_t channel, __u16* scale)
{
	if (channel & badChannels)
		return CAMAC_CC_INVALID_ARG;
	__u16 data;
	int rc = AF(afScalerRead, &data);
	*scale = ( data >> (ScalerLength*channel) ) & ScalerMask;
	return rc;
}

int ZIF_L0323::SetTime(ZIF_ch_t channel, float time)
{
	if ( time < minDuration || time > maxDuration )
		return CAMAC_CC_INVALID_ARG;

//	printf("I am SetTime(%d,%f)\n", channel, time);
	time /= DurationStep;
	__u16 sca = __u16(time+1.) >> DurationLength;
//	printf(" 1: dur=%f sca=%d\n", time,sca);
	sca = sca > 2 ? 2 : sca;
	__u16 dur = __u16( 0.5 + time / (1<<sca) );
//	printf(" 2: dur=%d sca=%d\n", dur,sca);
	return SetScaler(channel, sca) | SetDuration(channel, dur);
}

int ZIF_L0323::GetTime(ZIF_ch_t channel, float* time)
{ 
	__u16 sca, dur;
	int rc = GetScaler(channel, &sca) | GetDuration(channel, &dur);
	if( sca == ScalerProhibitValue )
		*time=-1.;
	else
		*time = (dur<<sca)*DurationStep;
	return rc;
}
