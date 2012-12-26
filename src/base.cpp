#include <camac/dfmodule/base.h>
#include "CamacServer.h"

#include <stdexcept>
#include <iostream>

#include <sys/time.h>
#include <assert.h>
#include <pthread.h>
//#include <camac/df/open_station.h>
//#include <dfcommonxx/bad.h>
//#include <dfc/xstrdup.h>	// :TODO: use dfString
//#include <dfc/xmalloc.h>	//
//#include <unistd.h>
//#include <errno.h>


using namespace Camac;
using namespace std;

Camac::AF convert(const camac_af_t & iAF) {
	Camac::AF rv;
	rv.a = camac_af_a(iAF);
	rv.f = camac_af_f(iAF);
	return rv;
}

void convertTimeoutToFedorov(Timeout left,  df_timeout_t * oTimeout) {
	if (oTimeout == DF_TIMEOUT_PTR_INF || oTimeout == DF_TIMEOUT_PTR_0)
		return;
	if (std::numeric_limits<Timeout>::infinity() == left) {
		*oTimeout = DF_TIMEOUT_INF;
		return;
	}
	*oTimeout = left * 1000;
}

Timeout convertTimeoutFromFedorov(const df_timeout_t * timeout) {
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
	try {
		Unbind();
		Server * server = Server::getDefault();
		assert(server);

		_module = &(getModule(server, addr.iface, addr.crate, addr.station));
		_station = addr.station;
	} catch (std::exception & e) {
		cerr << "Bind to address " << (int)addr.iface << "/" << 
(int)addr.crate << "/" << (int)addr.station << " failed: " << e.what() 
<< endl;
		return -1;
	}


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
	return _module->crate().C(convertTimeoutFromFedorov(timeout));
}

int dfCamacModuleBase::Z(df_timeout_t* timeout) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	return _module->crate().Z(convertTimeoutFromFedorov(timeout));
}

static double now() {
	timeval tv;
	if (gettimeofday(&tv, 0) != 0) {
		throw std::runtime_error("gettimeofday failed");
	}

	return tv.tv_sec + tv.tv_usec*1e-6;
}


int dfCamacModuleBase::WaitLAM(df_timeout_t* timeout) {
	if (!_module) {
		return CAMAC_CC_NOTBINDED;
	}
	Timeout t = convertTimeoutFromFedorov(timeout);
	if (t <= 0) {
		return _module->waitLAM(0);
	}
	double targetTime = t + now();
	while (targetTime >= now()) {
		int rv = _module->waitLAM(targetTime - now());
		Timeout time_left = targetTime - now();
		convertTimeoutToFedorov(time_left, timeout);
		if (rv & CAMAC_CC_ERRORS) {
			return rv;
		}
		if (!(rv & CAMAC_CC_BOOL)) // timeout
			return rv;
		if (!_lcm) {
			return CAMAC_CC_BOOL;
		} else {
			if (_lcm->method == CAMAC_LAM_CHECK_NONE)
				return CAMAC_CC_BOOL;

			u32_t tmp = 0;
			rv = 0;

			switch (_lcm->method) {
			case CAMAC_LAM_CHECK_BY_Q:
			case CAMAC_LAM_CHECK_BY_NOTQ:
				rv = AF(_lcm->af);
				break;
			case CAMAC_LAM_CHECK_BY_BITMASK:
			case CAMAC_LAM_CHECK_BY_NOTBITMASK:
				rv = AF(_lcm->af, &tmp);
				break;
			default:
				return CAMAC_CC_INVALID_ARG;
			}
			if (rv & CAMAC_CC_ERRORS)
				return rv;

			switch (_lcm->method) {
			case CAMAC_LAM_CHECK_BY_Q:
				if (rv & CAMAC_CC_NOT_Q)
					break;
				return CAMAC_CC_BOOL;
			case CAMAC_LAM_CHECK_BY_NOTQ:
				if (rv & CAMAC_CC_NOT_Q)
					return CAMAC_CC_BOOL;
				break;
			case CAMAC_LAM_CHECK_BY_BITMASK:
				if (tmp & _lcm->bitmask)
					return CAMAC_CC_BOOL;
				break;
			case CAMAC_LAM_CHECK_BY_NOTBITMASK:
				if (tmp & _lcm->bitmask)
					break;
				return CAMAC_CC_BOOL;
			default:
				return CAMAC_CC_INVALID_ARG;
			}
			continue;
		}
	}
	return CAMAC_CC_BOOL;
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
	return NULL;
}

LAMHandler::LAMHandler(dfCamacModuleBase & module, Callback cb, void * data):
		_module(module),
		_cb(cb),
		_data(data),
		exitFlag(false)
{
	if (pthread_create(&_thread, NULL, staticCallback, this)) {
		throw std::runtime_error("Thread creation failed.");
	}
}

LAMHandler::~LAMHandler() {
	exitFlag = true;
	void * rv;
	pthread_join(_thread, &rv);
}
