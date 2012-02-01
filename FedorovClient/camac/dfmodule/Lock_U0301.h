/*
 * Lock_U0301.h
 *
 *  Created on: 12.10.2010
 *      Author: gulevich
 */

#ifndef LOCK_U0301_H_
#define LOCK_U0301_H_

#include "camac/dfmodule/base.h"

class Lock_U0301: public dfCamacModuleBase {
	typedef dfCamacModuleBase Base;
public:
	Lock_U0301();
	enum
	{
		afReadI		= CAMAC_MAKE_AF(0, 0), // TODO: check
		afReset		= CAMAC_MAKE_AF(0, 8)  // TODO: check
	};

	//CAMAC_CC_NOT_Q is set if block has raised I
	int Read();
	int Reset();
	int Bind(const camac_address& _addr, df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int flags=0) {
		return Base::Bind(_addr, lock_station_timeout, flags);
	}
	int Unbind(void) {
		return Base::Unbind();
	}
	int Init  (void) {return Base::Init();}
};

#endif /* LOCK_U0301_H_ */
