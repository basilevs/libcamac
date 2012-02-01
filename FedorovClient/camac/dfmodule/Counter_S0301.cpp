/*
$Id: Counter_S0301.cpp,v 1.2 2003/01/15 08:20:24 barnykov Exp $
*/

#include "Counter_S0301.h"
#include <camac/df/rc2test.h>
#include <dfcommonxx/bad.h>

const char Counter_S0301::Type[]="Counter_S0301    ";

const struct camac_lam_check_method Counter_S0301::_lcm =
	{	afLAMcheck,	0,	CAMAC_LAM_CHECK_BY_Q };

Counter_S0301::Counter_S0301()
	: Base(Type, &_lcm)
{
}

int Counter_S0301::Init(void)
{
	int rc =  Base::Init();
	if (rc)
		return rc;

	for ( size_t i=0; i<nCh; i++)
		rc |= Reset(i);

	return rc;
}

dev_test_state Counter_S0301::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	if (dt!=DEV_TEST_OK) return dt;

	__u16 data;
	int rc = 0;
	for ( COUNT_ch_t i=0; i<nCh; i++)
		rc |= ReadClear(i, &data);
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	return DEV_TEST_OK;
}

const camac_af_t Counter_S0301::afRead[nCh] =
{
	CAMAC_INIT_AF(0, 0),	CAMAC_INIT_AF(1, 0),
	CAMAC_INIT_AF(2, 0),	CAMAC_INIT_AF(3, 0),
	CAMAC_INIT_AF(4, 0),	CAMAC_INIT_AF(5, 0),
	CAMAC_INIT_AF(6, 0),	CAMAC_INIT_AF(7, 0)
};

const camac_af_t Counter_S0301::afReadClear[nCh] =
{
	CAMAC_INIT_AF(0, 2),	CAMAC_INIT_AF(1, 2),
	CAMAC_INIT_AF(2, 2),	CAMAC_INIT_AF(3, 2),
	CAMAC_INIT_AF(4, 2),	CAMAC_INIT_AF(5, 2),
	CAMAC_INIT_AF(6, 2),	CAMAC_INIT_AF(7, 2)
};

const camac_af_t Counter_S0301::afIncrement[nCh] =
{
	CAMAC_INIT_AF(0, 25),	CAMAC_INIT_AF(1, 25),
	CAMAC_INIT_AF(2, 25),	CAMAC_INIT_AF(3, 25),
	CAMAC_INIT_AF(4, 25),	CAMAC_INIT_AF(5, 25),
	CAMAC_INIT_AF(6, 25),	CAMAC_INIT_AF(7, 25)
};

const camac_af_t Counter_S0301::afClear[nCh] =
{
	CAMAC_INIT_AF(0, 9),	CAMAC_INIT_AF(1, 9),
	CAMAC_INIT_AF(2, 9),	CAMAC_INIT_AF(3, 9),
	CAMAC_INIT_AF(4, 9),	CAMAC_INIT_AF(5, 9),
	CAMAC_INIT_AF(6, 9),	CAMAC_INIT_AF(7, 9)
};

int Counter_S0301::CZReset(df_timeout_t* timeout)
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

int Counter_S0301::Reset(COUNT_ch_t channel)
{
	return (AF(afClear[channel]) & ~CAMAC_CC_NOT_Q)
		| CompleteLAM();
}

/*int Counter_S0301::CZReset()
{
	int rcc=0,rcz=0;
	if( ( rcz=Z() ) && ( rcc=C() ) )
		return rcz | rcc;
	return rcz&rcc;
}
*/

//Прочитать спаренный счетчик без сброса.
//Возвращает camac_cc_bits.
int Counter_S0301::Read2(COUNT_ch_t channel, __u32* data)
{
	if ( channel & bad2Ch ) return CAMAC_CC_INVALID_ARG;
	__u16 hiWord, lowWord;
	int rc =
		AF(afRead[2*channel+1], &hiWord ) |
		AF(afRead[2*channel],   &lowWord) ;
	*data = __u32(hiWord)<<16 | lowWord;
	return rc;
}

//Прочитать спаренный счетчик со сбросом.
//Возвращает camac_return_code_bits.
int Counter_S0301::Read2clear(COUNT_ch_t channel, __u32* data)
{
	if ( channel & bad2Ch ) return CAMAC_CC_INVALID_ARG;
	__u16 hiWord, lowWord;
	int rc =
		AF(afRead     [2*channel+1], &hiWord )	|
		AF(afReadClear[2*channel  ], &lowWord)	|
		Reset(2*channel+1)						;
	*data = __u32(hiWord)<<16 | lowWord;
	return rc;
}

//Прочитать LAM регистр
//Возвращает camac_return_code_bits.
int Counter_S0301::ReadLAM(__u16* data)
{
	int rc = AF(afReadLAM, data);
	if (*data & badLAM)
		*data = bad(*data);
    return rc;
}

/*
$Log: Counter_S0301.cpp,v $
Revision 1.2  2003/01/15 08:20:24  barnykov
Some changes...

Revision 1.1.1.1  2001/03/24 14:55:31  barnykov
Переношу в правильный репозиторий...

Revision 1.1  2001/03/24 02:57:45  barnykov
Иду на Kedr-10.

*/
