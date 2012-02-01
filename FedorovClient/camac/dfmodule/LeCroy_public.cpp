
#include <stdio.h>
#include <unistd.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>

//#include <camac/kernel.h>

#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

#include "LeCroy.h"

const char LeCroy::Type[]="2132 HV Interface   ";

LeCroy::LeCroy()
	: Base(Type, &dfcamac_lcm_none)
{
	for( __u8 i=0; i < nChannels; i++ )
		VoltCode[i] = 0;
}

static camac_address addr;

int LeCroy::Init(__u8 mainframe)
{
	int rc=Base::Init()
		| GeneralZero()
		| EnableLAM();
	if( rc ) return rc;
	
	__u16 code;
	rc = ClearAllLAM();
	u16_t dummy;   type_tag dummy_tag;
	for ( __u8 i=0; i<nChannels; i++) {
        dummy=MakeCommandWord(i,mainframe,T_ReadSetVoltage);
		if( (rc|=Write(dummy)|Read(&dummy))&CAMAC_CC_ERRORS ) return rc;
		PartCommandVoltWord(dummy,&code,&dummy_tag);
		VoltCode[i] = code;
	}
	return rc;
}

dev_test_state LeCroy::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
    int rc=ClearAllLAM();
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);
/*re-write later
	for ( size_t i=0; i<nChannels; i++ )
	{
		if( (dt=dfcamac_rc2test(SetVoltage(i,0)|AF(afOff[i])))!=DEV_TEST_OK )
			return dt;

		bool on;

		if( (dt=dfcamac_rc2test(AF(afOn[i])|ReadState(i,&on)))!=DEV_TEST_OK )
			return dt;
		if ( !on )
		{
			printf("%s: Test error: Ch=%d unable to turn on \n"
				, Type, i );
			return DEV_TEST_BROKEN;
		}

		if( (dt=dfcamac_rc2test(AF(afOff[i])|ReadState(i,&on)))!=DEV_TEST_OK )
			return dt;
		if ( on )
		{
			printf("%s: Test error: Ch=%d unable to turn off \n"
				, Type, i );
			return DEV_TEST_BROKEN;
		}

		for(__u8 shift=0; shift<nVoltageBits; shift++)
		{
			__u16 w, r;
			w = 1<<shift;
			if( (dt=dfcamac_rc2test(AF(afWrite[i],&w)))!=DEV_TEST_OK )
				return dt;
			if( (dt=dfcamac_rc2test(ReadVoltage(i,&r)))!=DEV_TEST_OK )
				return dt;
			if (w!=r)
			{
				printf("%s: Test error: Ch=%2d  W=%2d  R=%2d\n"
					, Type, i, w, r);
				return DEV_TEST_BROKEN;
			}
		}
		if( (dt=dfcamac_rc2test(SetVoltage(i,0)|AF(afOn[i])))!=DEV_TEST_OK )
			return dt;

	}
	rc = AF(afCheckLAM);
	if ( rc & (CAMAC_CC_ERRORS & ~CAMAC_CC_NOT_Q) )
		return dfcamac_rc2test(rc & ~CAMAC_CC_NOT_Q);
	if ( !(rc & CAMAC_CC_NOT_Q) )
		return DEV_TEST_BROKEN;
*/
	return DEV_TEST_OK;
}

int LeCroy::Verify(__u8 mainframe)
{
	int rc=Base::Verify();
	if( rc ) return rc;
	
	__u16 code;
	u16_t dummy;   type_tag dummy_tag;
	for ( __u8 i=0; i<nChannels; i++)
	{
		dummy=MakeCommandWord(i,mainframe,T_ReadSetVoltage);
		if( (rc=Write(dummy)|Read(&dummy))&CAMAC_CC_ERRORS ) return rc;
		PartCommandVoltWord(dummy,&code,&dummy_tag);
		if( code != VoltCode[i] )
			return CAMAC_CC_VERIFICATION_ERROR;
	}

	return 0;
}

//-----------------------------------

int LeCroy::SetVoltage(__u8 channel, __u8 mainframe, const __u16 voltage)
{
	if ( channel >= nChannels ) return CAMAC_CC_INVALID_ARG;
    __u16 volt = voltage;
    bool flag;
    u16_t enable_finished_response = MakeCommandWord(static_cast<__u8>(ON), mainframe, T_EnDisFinished);
    u16_t modify_one_channel = MakeCommandWord(channel, mainframe, T_ModifyOne);
    u16_t set_volt_word  = MakeCommandVoltWord(volt, T_SetVoltage);

	int rc = 0
		| Write(enable_finished_response)    //where is Q?
		| Write(modify_one_channel);
    if(rc&CAMAC_CC_ERRORS) return rc;

    for( int i=0; i<VoltageLimit; i++ ){
		flag = ON;
		short type_lam;
		rc = 0
			| ClearInBuff()
	        | ClearLAM2()
			| Write(set_volt_word);
		if(rc&CAMAC_CC_ERRORS) return rc; //continue;
        rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

		if( type_lam>0 ){
            if( type_lam==1 )
            {
                printf("\tSetVoltage: LAM1 was got.\n");
				return AlarmLAM1;
            }
            else if( type_lam==2 )
            {
				u16_t data_word;
				__u8 dummy_mnfr, dummy_msc;
				type_tag tag;
				rc = 0
					| Read(&data_word);
				if(rc&CAMAC_CC_ERRORS) return rc; //continue;
                PartCommandWord(data_word, &dummy_msc, &dummy_mnfr, &tag);
		//if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
				if( tag!=T_EnDisFinished ){
                    printf("\tSetVoltage: incorrect tag.\n");
					flag = OFF;
					continue;
				} else{
					flag = ON;
                    break;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) ) return rc;

    if(flag)
		return 0;
	else
		return FailedTrans;
};

int LeCroy::SetAllVoltage(__u8 mainframe, const __u16 *data, short size){
    bool flag;
    short num = size;
    u16_t volt_word[num];
    __u16 volt[num];
	for( short i=0; i<num; i++ )
		volt[i] = data[i];

    u16_t enable_finished_response = MakeCommandWord(static_cast<__u8>(ON), mainframe, T_EnDisFinished);
    u16_t modify_all_channels = MakeCommandWord(0, mainframe, T_ModifyAll);
    for( short i=0; i<num; i++ )
		volt_word[i] = MakeCommandVoltWord(volt[i], T_SetVoltage);

	int rc = 0
		| Write(enable_finished_response);
	if( rc&CAMAC_CC_ERRORS ) return rc;

    for( short i=0; i<VoltageLimit; i++ ){
		flag = ON;
		rc = 0
			| ClearInBuff()
			| ClearLAM2()
			| Write(modify_all_channels);
		if( rc&CAMAC_CC_ERRORS ) return rc; //continue;
        bool write_flag=ON;
        for( short j=0; j<num; j++ )
        {
			rc |= Write(volt_word[j]);
            if( rc&CAMAC_CC_ERRORS )
            {
				printf("SetAllVoltage: error occured while writing volt_word[%d].\n", j);
				//return rc;
                write_flag = OFF;
                break;
			}
		}
		if( !write_flag ) continue;

		// LAM after end of all cycle
		short type_lam;
		rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

		if( type_lam>0 ){
            if( type_lam == 1 )
            {
                printf("\tSetAllVoltage: LAM1 was got.\n");
				return AlarmLAM1;
            } else if( type_lam == 2 )
            {
				u16_t data_word;
				__u8 dummy_mnfr, dummy_msc;
				type_tag tag;
				rc = 0
					| Read(&data_word);
				if( rc&CAMAC_CC_ERRORS ) return rc; //continue;
				PartCommandWord(data_word, &dummy_msc, &dummy_mnfr, &tag);
				//if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                if( tag!=T_EnDisFinished )
                {
                    printf("\tSetVoltage: incorrect tag.\n");
					flag = OFF;
					continue;
                } else
                {
					flag = ON;
                    break;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) ) return rc;
 
    if(flag)
		return 0;
    else
		return FailedTrans;
};

int LeCroy::GetVoltage(__u8 channel, __u8 mainframe, __u16 *voltage){
    bool flag;
    __u16 volt;
    type_tag tag;
    u16_t read_set_value = MakeCommandWord(channel, mainframe, T_ReadSetVoltage);

    for( short i=0; i<VoltageLimit; i++ )
    {
        flag = ON;
        short type_lam;
        int rc = 0
            | ClearInBuff()
            | ClearLAM2()
            | Write(read_set_value)
            | GetLAM(&type_lam);
        if(rc&CAMAC_CC_ERRORS) return rc;

        if( type_lam > 0 )
        {
            if( type_lam == 1 )
                return AlarmLAM1;
            else if( type_lam == 2 )
            {
                u16_t data_word;
                int rc = 0
                    | Read(&data_word);
                    //| ClearLAM2();
                if(rc&CAMAC_CC_ERRORS) return rc;
                PartCommandVoltWord(data_word, &volt, &tag);
                //if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                if( tag!=T_ReadSetVoltage ) {
                    flag = OFF;
                    continue;
                } else{ //else if( tag==T_ReadSetVoltage ){
                    *voltage = volt;
                    return 0;
                }
            }
        }
        else
        {
            return AnyOtherErr;
        }
    }

    if(flag)
        return 0;
    else
        return FailedTrans;
};

int LeCroy::GetAllVoltage(__u8 mainframe, __u16 *data, short size){
    short num = size, type_lam;
    bool flag;
    __u16 volt[num];
    for( short i=0; i<num; i++ )
        volt[i] = -1;
    u16_t read_all_set_values = MakeCommandWord(0, mainframe, T_ReadSetAll);

    for( int i=0; i<VoltageLimit; i++ )
    {
        flag = ON;
        int rc = 0
            | ClearInBuff()
            | ClearLAM2()
            | Write(read_all_set_values)
            | GetLAM(&type_lam);
        if(rc&CAMAC_CC_ERRORS) return rc;

        if( type_lam > 0 )
        {
            if( type_lam == 1 )
                return AlarmLAM1;
            else if( type_lam == 2 )
            {
                u16_t data_word;
                __u16 dummy_volt;
                short count=0;
                type_tag tag;
                int rc = 0;
                while(1)
                {
                    rc = Read(&data_word);
                    if(rc != 0)
                    {
                        if( rc == CAMAC_CC_NOT_Q ) break;
                        return rc;
                    }
                    PartCommandVoltWord(data_word, &dummy_volt, &tag);
                    //if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                    if( tag!=T_ReadSetVoltage ){
                        flag = OFF;
                        break;
                    } else //} else if( tag==T_ReadSetVoltage )
                        volt[count] = dummy_volt;

                    count++;
                }
                //rc = 0 | ClearLAM2();
                if( (count==num) && flag ){
                    for( short i=0; i<num; i++ )
                        data[i] = volt[i];
                    return 0;
                }
            }
        }
        else
        {
            return AnyOtherErr;
        }
    }

    if(flag)
        return 0;
    else
        return FailedTrans;
};

int LeCroy::GetTrueVoltage(__u8 channel, __u8 mainframe, __u16 *voltage){
    short type_lam;
    bool flag;
    u16_t read_true_voltage = MakeCommandWord(channel, mainframe, T_ReadTrueVoltage);
    int rc;

    for( int i=0; i<VoltageLimit; i++ )
    {
		flag = ON;
		rc = 0
			| ClearInBuff()
			| ClearLAM2()
			| Write(read_true_voltage);
        if( rc&CAMAC_CC_ERRORS ) continue;

        rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

        if( type_lam>0 )
        {
			if( type_lam == 1 ){
				return AlarmLAM1;
            } else if( type_lam == 2 )
            {
				u16_t data_word;
				__u16 volt;
				type_tag tag;
				int rc = 0
					| Read(&data_word);
                if( rc&CAMAC_CC_ERRORS ) continue;
				PartCommandVoltWord(data_word, &volt, &tag);
				if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
					flag = OFF;
					continue;
				} else if( tag==T_ReadTrueVoltage ){
					*voltage = volt;
					flag = ON;
                    break;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) )
        return rc;

    if( flag )
		return 0;
    else
		return FailedTrans;
};

int LeCroy::GetTrueAllVoltage(__u8 mainframe, __u16 *data, short size){
    short num = size, type_lam;
    __u16 volt[num];
    bool flag;
    for( short i=0; i<num; i++ ) volt[i] = -1;

    //u16_t enable_finished_response = MakeCommandWord(static_cast<__u8>(ON),mainframe,EnDisFinished);
    u16_t read_true_all_voltages = MakeCommandWord(0, mainframe, T_ReadTrueAll);
    int rc;

	/*rc |= Write(enable_finished_response);
    if(rc&CAMAC_CC_ERRORS) return rc;*/

    for( short i=0; i<VoltageLimit; i++ )
    {
		flag = ON;
		rc = 0
			| ClearInBuff();
		if( rc&CAMAC_CC_ERRORS ) continue;
		rc = 0
			| ClearLAM2();
		if( rc&CAMAC_CC_ERRORS ) continue;
		rc = 0
			| Write(read_true_all_voltages); // Q is true
		if( rc&CAMAC_CC_ERRORS ) continue;
		rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

        if( type_lam>0 )
        {
			if( type_lam == 1 ){
				return AlarmLAM1;
            } else if( type_lam == 2 )
            {
				u16_t data_word;
                __u16 dummy_volt;
                short count=0;
				type_tag tag;
				int rc = 0;
                while(1)
                {
					rc = 0
						| Read(&data_word);
					if( rc&CAMAC_CC_ERRORS ) {
						if( rc==CAMAC_CC_NOT_Q ) break;
						flag = OFF;
                        break;
					}
					PartCommandVoltWord(data_word, &dummy_volt, &tag);
					if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                        printf("\tAllCheckTrueU: parity, overwrite or transmission error.\n");
						flag = OFF;
						break;
					} else if( tag==T_ReadTrueVoltage ){
						volt[count] = dummy_volt;
						flag = ON;
					} else{
                        printf("\tAllCheckTrueU: some other error.\n");
						flag = OFF;
						break;
                    }
                    count++;
				}
                if( (count==num) && flag )
                {
					for( short i=0; i<num; i++ )
						data[i] = volt[i];
					break;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) )
        return rc;

    if( flag )
		return 0;
    else
		return FailedTrans;
};

int LeCroy::TurnOnOff(__u8 mainframe, const bool OnOff)
{
	__u8 dummy_mnfr, dummy_msc;
	short type_lam;
    bool flag;
    type_tag tag;
    u16_t turn_on_off_word = MakeCommandWord(static_cast<__u8>(OnOff), mainframe, T_HVOnOff);
    int rc;

    for( short i=0; i<TurnLimit; i++ )
    {
		flag = ON;
		rc = 0
			| ClearInBuff()
			| ClearLAM2()
			| Write(turn_on_off_word);
		if( rc&CAMAC_CC_ERRORS ){
            continue;
			//return rc;
		}
        rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

        if( type_lam>0 )
        {
			if( type_lam == 1 )
				return AlarmLAM1;
            else if( type_lam == 2 )
            {
				u16_t data_word;
				rc = 0
					| Read(&data_word);
				if( rc&CAMAC_CC_ERRORS ) continue;
				PartCommandWord(data_word, &dummy_msc, &dummy_mnfr, &tag);
				if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                    printf("\tTurn On-Off: incorrect tag.\n");
					flag = OFF;
					continue;
				} else{
					flag = ON;
                    break;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) )
        return rc;

    if( flag )
		return 0;
    else
		return FailedTrans;
};

int LeCroy::CheckTurnOn(__u8 mainframe, short *data, bool *stat){
    __u8 dummy_mnfr, stat_chan;
    short type_lam, ch[nChannels];
    bool flag, stat_on_off=ON;
    type_tag tag;
    for( short i=0; i<nChannels; i++ ) ch[i] = 1;
	u16_t get_stat_word = MakeCommandWord(0, mainframe, T_Status);
    int rc;

    for( short i=0; i<TurnLimit; i++ )
    {
		flag = ON;
		rc = 0
			| ClearInBuff()
			| ClearLAM2()
			| Write(get_stat_word);
		if( rc&CAMAC_CC_ERRORS ){
    		//return rc;
            continue;
		}
        rc = 0
			| GetLAM(&type_lam);
		if( rc<0 ) continue;

        if( type_lam>0 )
        {
			if( type_lam == 1 ){
				return AlarmLAM1;
            } else if( type_lam == 2 )
            {
				u16_t data_word;
                while(1)
                {
					rc = 0
						| Read(&data_word);
					if( rc&CAMAC_CC_ERRORS ){
                        if( rc==CAMAC_CC_NOT_Q ) break;
                        flag = OFF;
                        break;
					}
					PartCommandWord(data_word, &stat_chan, &dummy_mnfr, &tag);
					if( tag==T_ParityErr || tag==T_OverwriteErr || tag==T_TransmissionErr ){
                        printf("\tCheckTrueOn: incorrect tag.\n");
						flag = OFF;
                        break;
					} else if( tag==T_HVOnOff ){
                        printf("\tCheckTrueOn: HVOnOff tag received.\n");
						if( (stat_chan&SwitchMask)==1 )
							stat_on_off = ON;
						else
							stat_on_off = OFF;
						flag = ON;
					} else if( tag==T_Status ){
                        printf("\tCheckTrueOn: Status tag received.\n");
						ch[stat_chan] = -1;
                        flag = ON;
					}
				}
                printf("\tCheckTrueOn: all words were got.\n"); //* Debug message
                if( flag )
                {
					for( short i=0; i<nChannels; i++ )
						data[i] = ch[i];
					*stat = stat_on_off;
				}
			}
		}
	}

    if( rc&(CAMAC_CC_ERRORS&~CAMAC_CC_NOT_Q) )
        return rc;
	
    if( flag )
		return 0;
    else
		return FailedTrans;
};
