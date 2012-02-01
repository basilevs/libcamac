/*
$Id: QDC_C0312.cpp,v 1.1.1.1 2001/03/24 14:55:31 barnykov Exp $
*/

#include "QDC_C0312.h"
#include <camac/df/std_lcm.h>
#include <camac/df/rc2test.h>

const char QDC_C0312::Type[] = "QDC_C0312        ";

QDC_C0312::QDC_C0312()
	: Base(Type, &dfcamac_lcm_a0_f8_q)
{
}

int QDC_C0312::Init(void)
{
	return Base::Init() | Reset();
}

dev_test_state QDC_C0312::Test(void)
{
	dev_test_state dt = dev_test_to_intermediate( Base::Test() );
	int rc = Reset();
	if (dt!=DEV_TEST_OK) return dt;
	if (rc&CAMAC_CC_ERRORS) return dfcamac_rc2test(rc);

	return DEV_TEST_OK;
}

//-------------------------------

const camac_af_t QDC_C0312::afReadClearLAM[nChannels] =
{
	CAMAC_INIT_AF(0, 2),
	CAMAC_INIT_AF(1, 2),
	CAMAC_INIT_AF(2, 2),
	CAMAC_INIT_AF(3, 2)
};

const camac_af_t QDC_C0312::afRead[nChannels] =
{
	CAMAC_INIT_AF(0, 0),
	CAMAC_INIT_AF(1, 0),
	CAMAC_INIT_AF(2, 0),
	CAMAC_INIT_AF(3, 0)
};

struct camac_af_list QDC_C0312::read_all[] =
{
	{ CAMAC_INIT_AF16(0, 0), 0 },
	{ CAMAC_INIT_AF16(1, 0), 0 },
	{ CAMAC_INIT_AF16(2, 0), 0 },
	{ CAMAC_INIT_AF16(3, 0), 0 }
};
struct camac_af_list QDC_C0312::read_all_reset_lam[] =
{
	{ CAMAC_INIT_AF16(0, 0), 0 },
	{ CAMAC_INIT_AF16(1, 0), 0 },
	{ CAMAC_INIT_AF16(2, 0), 0 },
	{ CAMAC_INIT_AF16(3, 0), 0 },
	{ afLAMclear,			 0 }
};

int QDC_C0312::CZReset(df_timeout_t* timeout)
{
	int rcc = C(timeout), rcz = Z(timeout);
	return ( (rcc&&rcz)?(rcz|rcc):(rcz&rcc) ) | CompleteLAM();
}

//Прочитать результат измерения со сбросом LAMа
//Возвращает camac_return_code_bits, CAMAC_RC_NO_Q при неготовности.
//isbad(*data) при перегрузке.
/*
int QDC_C0312::ReadClearLAM(size_t channel, __u16* data)
{
	if ( channel & badChann ) return CAMAC_CC_INVALID_ARG;
	int rc = AF( afReadClearLAM[channel], data ) | CompleteLAM();
	*data = checkdata(*data);
	return rc;
}
*/
/*
int QDC_C0312::ReadClearLAM(__u16* const data)
{
	int rc =
		ListAF (read_all_reset_lam,
			sizeof(read_all_reset_lam)/sizeof(camac_af_list) ) |
		CompleteLAM();

	__u16* pdata = data;
	camac_af_list* plist = read_all_reset_lam;
	for(int i=0; i<nChannels; i++)
	return rc;
}
*/
//Прочитать результат измерения без сброса LAMа
//Возвращает camac_return_code_bits, CAMAC_RC_NO_Q при неготовности.
//isbad(*data) при перегрузке.
/*
int QDC_C0312::Read(size_t channel, __u16* data)
{
	if ( channel & badChann ) return CAMAC_CC_INVALID_ARG;
	int rc = AF( afRead[channel], data );
	*data = checkdata(*data);
	return rc;
}

*/
int QDC_C0312::read(const camac_af_t *af, QDC_ch_t channel, __u16* data)
{
	if ( channel & badChann ) return CAMAC_CC_INVALID_ARG;
	int rc = AF( af[channel], data );
	*data = checkdata(*data);
	return rc;
}

int QDC_C0312::read( camac_af_list* aflist, __u16* data)
{
//	camac_af_list* plist = aflist;
//	__u16* pdata = data;

	int rc = ListAF( aflist, sizeof(aflist)/sizeof(camac_af_list) );

	for(QDC_ch_t i=0; i<nChannels; i++)
//		*pdata++ = checkdata( (plist++)->data );
		data[i] = checkdata( (aflist++)->data );

	return rc;
}

/*
$Log: QDC_C0312.cpp,v $
Revision 1.1.1.1  2001/03/24 14:55:31  barnykov
Переношу в правильный репозиторий...

Revision 1.1  2001/03/24 02:57:46  barnykov
Иду на Kedr-10.

*/
