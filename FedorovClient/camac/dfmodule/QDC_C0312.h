#ifndef  _QDC_C0312_h
# define _QDC_C0312_h

#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <linux/types.h>

class QDC_C0312 : public dfCamacModuleBase
{
	typedef dfCamacModuleBase Base;
	typedef ushort QDC_ch_t;
	QDC_C0312(const QDC_C0312&);
	QDC_C0312& operator=(const QDC_C0312&);
public:
	static const char Type[];

	enum
	{
		nChannels = 0x0004,	maxChann  = 0x0003,	badChann  = ~maxChann,
		maxValue  = 0x0FFF,	badValue  = ~maxValue
	};

private:
	enum
	{
		afLAMcheck		= CAMAC_MAKE_AF(0, 8),
		afLAMclear		= CAMAC_MAKE_AF(0,10),
		afLAMdisable	= CAMAC_MAKE_AF(0,24),
		afLAMenable		= CAMAC_MAKE_AF(0,26)
	};

	static struct camac_af_list read_all[];
	static struct camac_af_list read_all_reset_lam[];

	static const camac_af_t afRead[nChannels];
	static const camac_af_t afReadClearLAM[nChannels];

public:
	QDC_C0312();
	inline ~QDC_C0312();

	inline int Bind(const camac_address& addr,
		df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0)
	{ return Base::Bind(addr, lock_station_timeout,
						flags	| CAMAC_OPEN_STATION_CZ_SENSITIVE
								| CAMAC_OPEN_STATION_LOCK_CZ );			}
	inline int Unbind(void)
		{ return Base::Unbind(); 					}

	inline int   Lock(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::LockStation(timeout); 		}
	inline int Unlock(void)
		{ return Base::UnlockStation(); 			}

public:
	int Init(void);
	dev_test_state Test(void);
	inline int Verify(void)	{ return Base::Verify();}

public:
	//������� ����� == C & Z
	//���������� camac_cc_bits.
	int			CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);
	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
    	{ return   Base::Lock(CAMAC_LOCK_CZ, timeout);	}
	inline int UnLockCZ(void)
		{ return Base::Unlock(CAMAC_LOCK_CZ);				}

private:
	//��������� ��������� ���������
	//���������� camac_return_code_bits, CAMAC_RC_NO_Q ��� ������������.
	//bad(*data) ��� ����������.
	//   ��������� AF-��
	int read( const camac_af_t    *af    , QDC_ch_t channel, __u16* data);
	//   ������� AF-��
	int read(		camac_af_list *aflist, __u16* data);

public:
	// ��� ������ LAM�
	// ���� �����
	inline int Read(QDC_ch_t channel, __u16* data)
		{ return read(afRead, channel, data ); 		}
	// ��� ������
	inline int Read(__u16* data)
		{ return read(read_all, 	   data );		}

	// �� ������� LAM�
	// ���� �����
	inline int ReadClearLAM(QDC_ch_t channel, __u16* data)
		{ return read(afReadClearLAM, channel, data ) | CompleteLAM(); }
	// ��� ������
	inline int ReadClearLAM(__u16* data)
		{ return read(read_all_reset_lam,	   data ) | CompleteLAM(); }

	//��������� LAM ���������� � ������ (����� ����������� A0 F8).
	//������ ���������� ��������������� � ������.
	//���������� L=Q.
	inline int CheckLAM(void)	{ return ~AF(afLAMcheck) & CAMAC_CC_NOT_Q; }

	//�������� ������ (�������) LAM � ������.
	//���������� camac_cc_bits & ~CAMAC_CC_NOT_Q .
	// F10 ������ Q=0 � ������� �� ����������� � ��������
	inline int Reset(void)		{ return (AF(afLAMclear) & ~CAMAC_CC_NOT_Q)
														| CompleteLAM(); }

	//��������� ������ �������� LAM.
	//���������� camac_cc_bits.
    /* �� �������� � ���� ����������� ������ */
	inline int DisableLAM(void)	{ return AF(afLAMdisable); }

	//��������� ������ �������� LAM.
	//���������� camac_cc_bits.
	inline int EnableLAM(void)	{ return AF(afLAMenable); }

	//-------------------------------

public:
	inline int SetEventsMask(ushort events_mask)
		{ return Base::SetEventsMask(events_mask); 	}
	inline int GetEvents	(void) const
		{ return Base::GetEvents(); 				}

	inline int SetI			(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
		{ return Base::SetAndLockI  (timeout);}
	inline int ClearI		(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF)
		{ return Base::ClearAndLockI(timeout);}
	inline int UnlockI		(void)
		{ return Base::UnlockI();				}

private:
	inline __u16 checkdata(const __u16 data) const
		{ return (data & badValue) ? bad(data) : data;	}
};


inline QDC_C0312::~QDC_C0312() { Unbind(); }

#endif	//_QDC_C0312_h

/*
$Log: QDC_C0312.h,v $
Revision 1.3  2003/01/15 08:20:24  barnykov
Some changes...

Revision 1.2  2001/05/29 13:14:01  barnykov
BMYuCamacModuleCommon added.

Revision 1.1.1.1  2001/03/24 14:55:31  barnykov
�������� � ���������� �����������...

Revision 1.1  2001/03/24 02:57:46  barnykov
��� �� Kedr-10.

*/
