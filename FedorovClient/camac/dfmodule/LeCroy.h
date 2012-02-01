/*
 $Id: LeCroy.h,v ?.? 2009/06/26 ?:?:? boroden Exp $
 $Id: LeCroy.h,v 1.3 2005/01/25 11:08:09 bobrov Exp $
*/
#ifndef _LeCroy_h
#define _LeCroy_h

//#include <camac/defs.h>
//#include <camac/df/parse_address.h>

#include <linux/types.h>
#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <dfcommon/bool.h>

class LeCroy : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;

	typedef unsigned int uint;

public:
    static const int AnyOtherErr=CAMAC_CC_USER_ERROR0;
    static const int AlarmLAM1  =CAMAC_CC_USER_ERROR1;
    static const int FailedTrans=CAMAC_CC_USER_ERROR2;

    static const char Type[];
	enum Tag{
    T_ModifyOne=0, T_SetVoltage=1, T_ModifyAll=2, T_ReadTrueVoltage=3,T_ReadTrueAll=4,
    T_HVOnOff=5, T_Restore=6, T_Preset=7, T_ReadSetVoltage=8, T_Status=9, T_ReadSetAll=10,
    T_EnDisFinished=11, T_ParityErr=12, T_OverwriteErr=13, T_Zero=14, T_TransmissionErr=15
	};
	typedef enum Tag type_tag;

    enum { nChannels = 32, PollingLimit = 100, VoltageLimit=5,
        TurnLimit=5,TagMask=0xF, MainframeMask=0x3F, DataMask=0xFFFF,
		LAM1Mask=0x1, LAM2Mask=0x2, SwitchMask=0x1
		};
	static const bool ON = true;
	static const bool OFF = false;

private:
//	static short nChannels = 32;
	__u16 VoltCode[nChannels];

	static const camac_af_t afReadLAM;
	static const camac_af_t afReadBuff;
	static const camac_af_t afClearBuffLAM;
	static const camac_af_t afClearInBuff;
	static const camac_af_t afWrite;
	static const camac_af_t afCheckLAM[2];
	static const camac_af_t afClearLAM[2];
	static const camac_af_t afEnableLAM[2];
	static const camac_af_t afDisableLAM[2];

public:
			LeCroy();
	inline ~LeCroy();

	inline int   Bind(const camac_address& addr,
	    df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
		{ return Base::Bind(addr, lock_station_timeout,
			flags &	~( CAMAC_OPEN_STATION_CZ_SENSITIVE
				   | CAMAC_OPEN_STATION_LOCK_CZ )  );	}
	inline int Unbind(void)
		{ return Base::Unbind(); 				}

	inline int   Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 			}
	inline int Unlock(void)
		{ return Base::UnlockStation(); 			}

	int Init(__u8 mainframe);
	dev_test_state Test(void);
	int Verify(__u8 mainframe);

public:
	int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); }
	int GetEvents    (void) const
		{ return Base::GetEvents(); }

private:
//generate command word from input parts:
	u16_t MakeCommandWord(__u8 misc, __u8 mainframe, type_tag tag);
//generate command word (for set voltage) from input parts:
	u16_t MakeCommandVoltWord(__u16 volt, type_tag tag);
//divide command word to output parts:
	void PartCommandWord(const u16_t input_word, __u8 *misc, __u8 *mainframe, type_tag *tag);
//divide command word (for voltage response) to output parts:
	void PartCommandVoltWord(const u16_t input_word, __u16 *volt, type_tag *tag);

	int Write(u16_t data_word);	// - write data_word to HV4032A
	int Read(u16_t *data_word);	// - read data_word from HV4032A
	int GetLAM(short *type);   	// - wait for LAM from HV4032A
						//LAM1=1, LAM2=2, PollingLimit exceeded=-1
	int ClearLAM2(void);		// - clear LAM2
	int ClearAllLAM(void);		// - clear LAM1 and LAM2
	int ClearInBuff(void);		// - clear input buffer
	int GeneralClear(void);		// - clear LAM12 and buffers
	int GeneralZero(void);		// - C + enable LAM1, disable LAM2
	int EnableLAM(void);		// - enable LAM1, enable LAM2

public:
//modify one voltage:
    int SetVoltage(__u8 channel, __u8 mainframe, const __u16 voltage);
//modify voltage in all channels:
    int SetAllVoltage(__u8 mainframe, const __u16 *data, short size);
//read one voltage code:
    int GetVoltage(__u8 channel, __u8 mainframe, __u16 *voltage);
//read voltage codes in all channels:
    int GetAllVoltage(__u8 mainframe, __u16 *data, short size);
//read real voltage in one channel:
    int GetTrueVoltage(__u8 channel, __u8 mainframe, __u16 *voltage);
//read real voltages in all channels:
    int GetTrueAllVoltage(__u8 mainframe, __u16 *data, short size);

//turn all channels on or off:
    int TurnOnOff(__u8 mainframe, const bool OnOff);
//check channels; data[i]=-1 for failed channel, =1 for working channel;
//stat indicates if they're turned on:
    int CheckTurnOn(__u8 mainframe, short *data, bool *stat);

};

inline LeCroy::~LeCroy() { Unbind();}

#endif	//_LeCroy_h



/*
 $Log: LeCroy.h,v $
 Revision ?.?  2009/06/26 ?:?:?  boroden
	broken
 $Log: LeCroy.h,v $
 Revision 1.3  2005/01/25 11:08:09  bobrov
 Перенос в правильный репозиторий.

*/

