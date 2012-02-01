#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
#include <fcntl.h>

//#include <camac/kernel.h>
//#include <camac/df/wait_lam.h>

#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

#include "LeCroy.h"

const camac_af_t LeCroy::afReadLAM      = CAMAC_INIT_AF(0, 0);
const camac_af_t LeCroy::afReadBuff     = CAMAC_INIT_AF(0, 2);
const camac_af_t LeCroy::afClearBuffLAM = CAMAC_INIT_AF(0, 9);
const camac_af_t LeCroy::afClearInBuff  = CAMAC_INIT_AF(1, 9);
const camac_af_t LeCroy::afWrite        = CAMAC_INIT_AF(0, 16);
const camac_af_t LeCroy::afCheckLAM[2]   =
				{ CAMAC_INIT_AF(0, 27), CAMAC_INIT_AF(1, 27) };
const camac_af_t LeCroy::afClearLAM[2]   =
				{ CAMAC_INIT_AF(0, 10), CAMAC_INIT_AF(1, 10) };
const camac_af_t LeCroy::afEnableLAM[2]  =
				{ CAMAC_INIT_AF(0, 26), CAMAC_INIT_AF(1, 26) };
const camac_af_t LeCroy::afDisableLAM[2] =
				{ CAMAC_INIT_AF(0, 24), CAMAC_INIT_AF(1, 24) };

int LeCroy::Write(u16_t data_word){
    u16_t data = data_word;

    int rc = 0
		| AF(afWrite, &data);
    return rc;
};

int LeCroy::Read(u16_t *data_word){
    u16_t data = DataMask;

    int rc = 0
		| AF(afReadBuff, &data);
    *data_word = data;
    return rc;
};

int LeCroy::GetLAM(short *type){
    u16_t data;
	short flag=-1;

	int rc = 0;

//--------polling----------------
	int count = 0;
	do{
        rc = 0
			| AF(afReadLAM, &data)&~CAMAC_CC_NOT_Q;
		if(rc) return rc;
		if( data&LAM1Mask )
		{
			flag=1;
            printf("LAM1 by polling was got.\n");
			break;
		}
		else if( data&LAM2Mask )
		{
			flag=2;
            printf("LAM2 by polling was got.\n");
			break;
		}
		printf("LAM by polling, cycle: %d (data: %d)\n",
										count, data);
	} while(count++<PollingLimit);
//--------polling_end------------
/*
//----------raw_camac------------
	//(, int time_wait)
	int wait = time_wait;
	df_timeout_t t_o = TIMEOUT*time_wait;
	
	if( (rc=dfcamac_wait_lam( GetFD(), &t_o ))<0 ){
        printf("Error while waiting LAM occured.\n");
        return rc;
	} else{
		printf("Waiting LAM complete.\n");
		//Read(&data);
		//printf("data_word: %d\n", data);
		rc = 0
			| AF(afReadLAM, &data)&~CAMAC_CC_NOT_Q;
		if( rc&CAMAC_CC_ERRORS )
			return rc;
		printf("Reading LAM complete.\n");
	}

    if( data&LAM1Mask )
		flag=1;
    else if( data&LAM2Mask )
		flag=2;
    else
		flag=-1;
	printf("data: %d\n", data);

	rc |= camac_kernel_complete_lam( GetFD() );
	if( rc&CAMAC_CC_ERRORS )
		return rc;
	printf("Complete LAM finished.\n");
//--------raw_camac_end----------
*/
	*type = flag;

    return rc;
};

int LeCroy::ClearLAM2(void){
    int rc = 0
		| AF(afClearLAM[1])&~CAMAC_CC_NOT_Q;
//	| camac_kernel_complete_lam( GetFD() );

    return rc;
};

int LeCroy::ClearAllLAM(void){
    int rc = 0
		| AF(afClearLAM[0])&~CAMAC_CC_NOT_Q
		| AF(afClearLAM[1])&~CAMAC_CC_NOT_Q;
//	| camac_kernel_complete_lam( GetFD() );

    return rc;
};

int LeCroy::ClearInBuff(void){
    int rc = 0
		| AF(afClearInBuff)&~CAMAC_CC_NOT_Q;

    return rc;
};

int LeCroy::GeneralClear(void){
    int rc = 0
		| AF(afClearBuffLAM)&~CAMAC_CC_NOT_Q;
//	| camac_kernel_complete_lam( GetFD() );

    return rc;
};

int LeCroy::GeneralZero(void){
    int rc = 0
		| AF(afClearBuffLAM)&~CAMAC_CC_NOT_Q
		| AF(afEnableLAM[0])&~CAMAC_CC_NOT_Q
		| AF(afDisableLAM[1])&~CAMAC_CC_NOT_Q;
//	| camac_kernel_complete_lam( GetFD() );

    return rc;
};

int LeCroy::EnableLAM(void){
    int rc = 0
	| AF(afEnableLAM[0])&~CAMAC_CC_NOT_Q
	| AF(afEnableLAM[1])&~CAMAC_CC_NOT_Q;

    return rc;
};

//**********************************************//
u16_t LeCroy::MakeCommandWord(__u8 misc, __u8 mainframe, type_tag tag){
    u16_t output_word;
				//16-11			10-5			4-1 bits
    output_word = (misc<<10) + (mainframe<<4) + tag;
    return output_word;
};

u16_t LeCroy::MakeCommandVoltWord(__u16 volt, type_tag tag){
    u16_t output_word;
				//16-5		4-1 bits
    output_word = (volt<<4) + tag;
    return output_word;
};

void LeCroy::PartCommandWord(const u16_t input_word, __u8 *misc, __u8 *mainframe, type_tag *tag){
    u16_t dummy_word = input_word;

    *tag = static_cast<type_tag>(dummy_word&TagMask);
    *mainframe = (dummy_word>>4)&MainframeMask;
    *misc = dummy_word>>10;
};

void LeCroy::PartCommandVoltWord(const u16_t input_word, __u16 *volt, type_tag *tag){
    u16_t dummy_word = input_word;

    *tag = static_cast<type_tag>(dummy_word&TagMask);
    *volt = dummy_word>>4;
};
