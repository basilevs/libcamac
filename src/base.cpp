/*
	<camac/dfmodule/base.cc>
	$Id: base.cc,v 1.2 2009-12-15 06:08:15 gulevich Exp $

	CAMAC module base class

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#include <camac/dfmodule/base.h>
#include "CamacServer.h"
#include <assert.h>
#include <pthread.h>
//#include <camac/df/open_station.h>
//#include <dfcommonxx/bad.h>
//#include <dfc/xstrdup.h>	// :TODO: use dfString
//#include <dfc/xmalloc.h>	//
//#include <unistd.h>
//#include <errno.h>


using namespace Camac;

Camac::AF convert(const camac_af_t & iAF) {
	Camac::AF rv;
	rv.a = camac_af_a(iAF);
	rv.f = camac_af_f(iAF);
	return rv;
}

Timeout convert(const df_timeout_t * timeout) {
	if (timeout == DF_TIMEOUT_PTR_INF)
		return std::numeric_limits<Timeout>::infinity();
	if (timeout == DF_TIMEOUT_PTR_0)
		return 0;
	if (!timeout) return 0;
	if (*timeout == DF_TIMEOUT_INF)
		return std::numeric_limits<Timeout>::infinity();
	if (*timeout == DF_TIMEOUT_0)
		return 0;
	return *timeout / 1000;
}

dfCamacModuleBase::dfCamacModuleBase(const char* _type,
				     const camac_lam_check_method* lcm,
				     const char*, void*,
				     size_t)
{
	_lcm = lcm;
	_module = 0;
}

dfCamacModuleBase::~dfCamacModuleBase()
{
    Unbind();
}

int dfCamacModuleBase::Bind(const camac_address& addr, df_timeout_t* , int)
{
	Unbind();
	Server * server = Server::getDefault();
	assert(server);
	_module = &(getModule(server, addr.iface, addr.crate, addr.station));
	_station = addr.station;

	//TODO: add thread safety
    return CAMAC_CC_OK;
}



//return cc_bits, unbinds anyway, see close(2)
int dfCamacModuleBase::Unbind(void)
{
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}

	for (AfList::iterator i = _onCloseAfList.begin(); i!=_onCloseAfList.end(); ++i) {
		AF(i->af, &i->data);
	}

	_module = 0;
	return CAMAC_CC_OK;
}

int dfCamacModuleBase::ListAF(camac_af_list* aflist, size_t count) {
	Result rv = CAMAC_CC_OK;
	for (size_t i = 0; i < count ; ++i) {
		rv |= AF(aflist[i].af, &aflist[i].data);
	}
	return rv;
}

int  dfCamacModuleBase::AF(camac_af_t af, u16_t* data) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	Result rv;
	if (is_camac_af_read(af)) {
		rv = _module->AFR(convert(af), *data);
	} else {
		rv = _module->AFW(convert(af), *data);
	}
	return rv;
}

int  dfCamacModuleBase::AF(camac_af_t af, u32_t* data) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	Result rv;

	if (af & CAMAC_AF_W_MASK) {
		if (is_camac_af_read(af)) {
			assert(data);
			rv = _module->AFR(convert(af), *data);
		} else {
			rv = _module->AFW(convert(af), *data);
		}
	} else {
		u16_t temp = *data;
		if (is_camac_af_read(af)) {
			assert(data);
			rv = _module->AFR(convert(af), temp);
			*data = temp;
		} else {
			rv = _module->AFW(convert(af), temp);
		}
	}
	return rv;
}

int dfCamacModuleBase::AF(camac_af_t af) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	return _module->AFC(convert(af));
}



int dfCamacModuleBase::C(df_timeout_t* timeout) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	return _module->crate().C(convert(timeout));
}

int dfCamacModuleBase::Z(df_timeout_t* timeout) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	return _module->crate().Z(convert(timeout));
}

int dfCamacModuleBase::WaitLAM(df_timeout_t* timeout) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	Timeout t = convert(timeout);
	return _module->waitLAM(t);
}

typedef LAMHandler::Callback Callback;
void * LAMHandler::staticCallback(void * a) {
	LAMHandler * h = static_cast<LAMHandler *>(a);
	while (!h->exitFlag) {
		df_timeout_t t = 1000;
		h->_module.WaitLAM(&t);
		if (h->exitFlag)
			break;
		h->_cb(h->_data);
	}
	NULL;
}

LAMHandler::LAMHandler(dfCamacModuleBase & module, Callback cb, void * data):
		_module(module),
		_cb(cb),
		_data(data),
		exitFlag(false)
{
	int result = pthread_create(&_thread, NULL, staticCallback, this);
}

LAMHandler::~LAMHandler() {
	exitFlag = true;
	void * rv = rv;
	pthread_join(_thread, &rv);
}
