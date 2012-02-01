#ifndef  _TDC_C0309_h
# define _TDC_C0309_h

#include <camac/dfmodule/base.h>
#include <dfcommonxx/bad.h>
#include <linux/types.h>

class TDC_C0309 : public dfCamacModuleBase
{
public:
	typedef dfCamacModuleBase Base;
	typedef ushort TDC_ch_t;
	static const char Type[];
	enum
	{
		nChannels = 4,	maxChann  = 0x0003,	badChann  = ~maxChann,
		maxValue  = 0x0FFF,	badValue  = ~maxValue,
		read_all_aflist_size=4, read_all_reset_lam_aflist_size=4
	};

private:
	enum
	{
		afLAMcheck		= CAMAC_MAKE_AF(0, 8),
		afLAMclear		= CAMAC_MAKE_AF(0,10)
//		afLAMdisable	= CAMAC_MAKE_AF(0,24),
//		afLAMenable		= CAMAC_MAKE_AF(0,26)
	};

	static struct camac_af_list read_all[];
	static struct camac_af_list read_all_reset_lam[];

	static const camac_af_t afRead[nChannels];
	static const camac_af_t afReadClearLAM[nChannels];

public:
			TDC_C0309();
	inline ~TDC_C0309();

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
	//БЫСТРЫЙ СБРОС == C & Z
	//Возвращает camac_cc_bits.
	int			CZReset(df_timeout_t* timeout=DF_TIMEOUT_PTR_0);
	inline int   LockCZ(df_timeout_t* timeout=DF_TIMEOUT_PTR_0)
    	{ return   Base::Lock(CAMAC_LOCK_CZ, timeout);	}
	inline int UnLockCZ(void)
		{ return Base::Unlock(CAMAC_LOCK_CZ);				}

private:
	//Прочитать результат измерения
	//Возвращает camac_return_code_bits, CAMAC_RC_NO_Q при неготовности.
	//bad(*data) при перегрузке.
	//   Отдельным AF-ом
	int read( const camac_af_t  *af, TDC_ch_t  channel, __u16* data);
	//   Списком AF-ов
	int read( camac_af_list *aflist, size_t aflist_size, __u16* data);

public:
	// Без сброса LAMа
	// один канал
	inline int Read(TDC_ch_t channel, __u16* data)
		{ return read(afRead, channel, data ); 		}
	// все каналы
	inline int Read(__u16* data)
		{ return read(read_all, read_all_aflist_size, data );		}

	// Со сбросом LAMа
	// один канал
	inline int ReadClearLAM(TDC_ch_t channel, __u16* data)
	  { return read(afReadClearLAM, channel, data ) | CompleteLAM(); }
	// все каналы
	inline int ReadClearLAM(__u16* data)
		{ return read(read_all_reset_lam,
					  read_all_reset_lam_aflist_size, data ) | CompleteLAM(); }

	//Проверить LAM обращением к модулю (часто встречается A0 F8).
	//Всегда обращается непосредственно к модулю.
	//Возвращает L=Q.
	inline int CheckLAM(void)	{ return ~AF(afLAMcheck) & CAMAC_CC_NOT_Q; }

	//Сбросить запрос (причину) LAM в модуле.
	//Возвращает camac_cc_bits & ~CAMAC_CC_NOT_Q .
	// F10 выдает Q=0 в отличие от заявленного в описании
	inline int Reset(void)		{ return (AF(afLAMclear) & ~CAMAC_CC_NOT_Q)
														| CompleteLAM(); }

	//Запретить модулю выдавать LAM.
	//Возвращает camac_cc_bits.
    /* Не работает в двух проверенных блоках */
//	inline int DisableLAM(void)	{ return AF(afLAMdisable); }

	//Разрешить модулю выдавать LAM.
	//Возвращает camac_cc_bits.
//	inline int EnableLAM(void)	{ return AF(afLAMenable); }

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


inline TDC_C0309::~TDC_C0309() { Unbind(); }

#endif	//_TDC_C0309_h
