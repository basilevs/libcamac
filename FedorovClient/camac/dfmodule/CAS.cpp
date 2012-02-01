
#include "CAS.h"
#include <stdio.h>
#include <camac/df/rc2test.h>
#include <camac/df/std_lcm.h>
//#include <camac/df/print_errors.h>

const char CAS::Type[]="CAS";
const ulong CAS::WaitSwitchTime[2] = {3000,8};

CAS::CAS(const char* _type, __u8 _nDWChannels, ChipSet _cs, Mode _mode, const camac_lam_check_method* _lcm)
	: Base(_type, _lcm),
	maxChannel    ( (_nDWChannels*_mode)-1 ),
    type          ( _type ),
	nDWChannels   ( _nDWChannels ),
	cs            ( _cs ),
	Channel       ( notValidChannel ),
	mode          ( _mode )
{}

CAS::~CAS(void)
   	{ Unbind(); }

int CAS::Init(__u8 ch)
{
	Channel = notValidChannel;
	return Base::Init() | Switch(ch);
}

dev_test_state CAS::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	__u8 Saved = Channel;
	int rc=0;
//	printf("maxCh=%d mode=%d cs=%d\n",
//		maxChannel,mode,cs);
	for ( __u8 ch=0; ch<nDWChannels*mode; ch++)
	{
		__u8 data;
		rc |= Switch(ch) | Read(&data);
//		if ( rc&CAMAC_CC_ERRORS ) return dfcamac_rc2test(rc);
		if ( data!=ch ) {
			fprintf( stderr, "%s: Written channel=%x\tRead channel=%x\n"
				,type, ch, data );
			dt = DEV_TEST_BROKEN;
		}
	}
	rc |= Switch(Saved);

	return ( rc&CAMAC_CC_ERRORS ) ? dfcamac_rc2test(rc) : dt;
}

int CAS::Verify(void)
{
	int rc=Base::Verify();
	if( rc ) return rc;

	__u8 ch;
	rc |= Read(&ch);
	if( rc&CAMAC_CC_ERRORS ) return rc;
	if( ch != Channel ) return CAMAC_CC_VERIFICATION_ERROR;
	return rc;
}

int CAS::Switch(__u8 ch)
{
	if (ch>maxChannel) return CHANNEL_NOT_IN_RANGE;
	u16_t data = ch;
	int rc = AF(afSwitch,&data);
	if( !rc )
		Channel = data;
	return rc;
}

int CAS::Read(__u8* ch)
{
	u16_t data;
	int rc=AF(afRead, &data);
	if ( rc&CAMAC_CC_ERRORS ) return rc;
    if( data > maxChannel ) return CHANNEL_NOT_IN_RANGE;
	*ch = maxChannel & data; //maximum channel number coincides with mask for valid channels
	return rc;
}
/*
int CAS::PrintErrors(const char* str, int rc)
{
	fprintf(stderr,"%s %s ", type, str);
	if( rc ) {
		fprintf(stderr,"\a");
		if ( rc > 0 ) {
			fprintf(stderr,": ");
			if( rc & CHANNEL_NOT_IN_RANGE ) {
				fprintf(stderr,"Wrong channel value\n");
				rc &= ~CHANNEL_NOT_IN_RANGE;
			}
            if( rc )
				dfcamac_print_errors(stderr,rc);
		}
		else
			perror(str);
	}
    else
		fprintf(stderr,": Success\n");
	return rc;
}
*/
