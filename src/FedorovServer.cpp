#include "FedorovServer.h"

#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <fcntl.h>

# include <camac/kernel.h>

#include "actools.h"

using namespace std;

#ifndef MULTIPLE_CAMAC_SERVERS
Camac::Fedorov::Server fedServer;
Camac::Server * Camac::Server::_default = &fedServer;
#endif

static df_timeout_t toFedorovTimeout(Camac::Timeout t) {
	if (isinf(t)) {
		return DF_TIMEOUT_INF;
	}
	if (t < 0 )
		return DF_TIMEOUT_0;
	return df_timeout_t(t * 1000.);
}




int dfcamac_open_station(const struct camac_address* addr,
			 const char* type,
			 const struct camac_lam_check_method* lcm,
			 const char* signature, void* shmptr, size_t shmsize,
			 df_timeout_t* lock_station_timeout, int flags)
{
    int _fd;
    int devfd=open("/dev/camac", O_RDWR);
    if (devfd<0)
	return devfd;

    _fd=camac_kernel_open_station(devfd, addr, type, lcm,
				   signature, shmptr, shmsize,  lock_station_timeout, flags);

    close(devfd);

    return _fd;
}

Camac::Result Camac::Fedorov::Module::bind() {
	if (_fd >= 0)
		return true;
	const struct camac_lam_check_method dfcamac_lcm_none =
	{
		  CAMAC_INIT_AF(0,0), 0, CAMAC_LAM_CHECK_NONE
	};

	camac_address addr;
  addr.driver_name = "k0607-lsi6";
	addr.station = _idx;
	addr.crate       = _crate._idx;
	addr.iface       = _crate._interface._idx;

	_fd = dfcamac_open_station( &addr, "AbstractCamac", &dfcamac_lcm_none, 0, 0, 0, DF_TIMEOUT_PTR_0, 0);
	if (_fd >= 0) {
		df_timeout_t t = toFedorovTimeout(1);
		if (camac_kernel_init(_fd)!=0) {
			close(_fd);
			_fd = -1;
			return CC_NOTINITED;
		}
		return 0;
	}
	return CC_SYSCALL_ERROR;
}

Camac::Fedorov::Module::Module(Crate &crate, int idx):
	_crate(crate),
	_idx(idx),
	_fd(-1)
{
}

Camac::Crate & Camac::Fedorov::Module::crate() const {
	return _crate;
}


static camac_af_t toFedorovAf(const Camac::AF & af)
{
	return CAMAC_INIT_AF(af.a,af.f);
}
static camac_af_t toFedorovAf16(const Camac::AF & af)
{
	return CAMAC_INIT_AF16(af.a,af.f);
}
static camac_af_t toFedorovAf32(const Camac::AF & af)
{
	return CAMAC_INIT_AF24(af.a,af.f);
}


Camac::Result Camac::Fedorov::Module::AFW(const AF & af, u16_t data) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	return camac_kernel_af16(_fd, toFedorovAf16(af), &data);
}
Camac::Result Camac::Fedorov::Module::AFW(const AF & af, u32_t data) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	return camac_kernel_af24(_fd, toFedorovAf32(af), &data);
}
Camac::Result Camac::Fedorov::Module::AFR(const AF & af, u16_t & data) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	return camac_kernel_af16(_fd, toFedorovAf16(af), &data);
}

Camac::Result Camac::Fedorov::Module::AFR(const AF & af, u32_t & data) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	return camac_kernel_af24(_fd, toFedorovAf32(af), &data);
}
Camac::Result Camac::Fedorov::Module::AFC(const AF & af) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	return camac_kernel_af(_fd, toFedorovAf(af));
}

static int restore_events_mask(int stfd, int saved_mask, int rc)
{
    if (saved_mask & CAMAC_MASKABLE_EVENTS)
	camac_kernel_set_events_mask(stfd, saved_mask);

    return rc;
}
static df_timeout_t nonulltimeval2timeout(const struct timeval* tv,
				   const size_t timeouts_per_sec)
{
    return
	tv->tv_sec  * timeouts_per_sec +
	tv->tv_usec / timeouts_per_sec;
}

static struct timeval* timeout2nonulltimeval(struct timeval* tv,
				      const df_timeout_t timeout,
				      const size_t timeouts_per_sec)
{
    tv->tv_sec  =  timeout/timeouts_per_sec;
    tv->tv_usec = (timeout%timeouts_per_sec)*timeouts_per_sec;
    return tv;
}

static struct timeval* timeoutptr2timeval(struct timeval* tv,
				   df_timeout_t* timeoutptr,
				   size_t timeouts_per_sec)
{
    df_timeout_t timeout = df_timeout_from_ptr(timeoutptr);

    if      (timeout==DF_TIMEOUT_INF)
    {
	return NULL;
    }
    else if (timeout==DF_TIMEOUT_0)
    {
	tv->tv_sec=0; tv->tv_usec=0;
	return tv;
    }

    return timeout2nonulltimeval(tv, timeout, timeouts_per_sec);
}


/*
Wait specified events on CAMAC station descriptor.
df_timeout_t* timeout in milliseconds, see <dfcommon/timeout.h>
Returns events mask or -1 on error (errno).
*/
static int dfcamac_wait_events(int stfd, ushort events_mask, df_timeout_t* timeoutptr)
{
    int rc;
    int saved_mask;
    fd_set fdset;
    struct timeval  tv;
    struct timeval* tvp = timeoutptr2timeval(&tv, timeoutptr, 1000);


    if ( (saved_mask=camac_kernel_get_events_mask(stfd)) < 0 )
	return saved_mask;

    if ( ( saved_mask&CAMAC_MASKABLE_EVENTS) !=
	(events_mask&CAMAC_MASKABLE_EVENTS) )
    {
	if ( (rc=camac_kernel_set_events_mask(stfd, events_mask)) < 0 )
	    return restore_events_mask(stfd, saved_mask, rc);
    }


    FD_ZERO(&fdset); FD_SET(stfd, &fdset);

    rc = select(stfd+1, NULL, NULL, &fdset, tvp);
    if (rc>=0)
    {
	if (FD_ISSET(stfd, &fdset))
	    rc = camac_kernel_get_events(stfd);
	else
	    rc = 0;
    }


    if (   is_df_timeout_ptr_valid(timeoutptr)
	&& *timeoutptr!=DF_TIMEOUT_0
	&& *timeoutptr!=DF_TIMEOUT_INF )
    {
	*timeoutptr = nonulltimeval2timeout(tvp, 1000);
    }


    return restore_events_mask(stfd, saved_mask, rc);
}

static int dfcamac_wait_lam(int fd, df_timeout_t* timeout)
{
	int rc = dfcamac_wait_events(fd, 0, timeout);
	if (rc>0) {
		if (rc&CAMAC_LAM_EVENT) {
			rc = CAMAC_CC_BOOL;
		}	else if (rc&CAMAC_ERRORS_EVENT)	{
			rc = camac_kernel_get_errors(fd);
		}
  } else if (rc==0) {
		errno = ETIME;
		rc = -1;
	}
	return rc;
}

Camac::Result Camac::Fedorov::Module::WaitLAM(Timeout timeout) {
	Camac::Result rc = bind();
	if (rc & CC_ERRORS)
		return rc;
	df_timeout_t t = toFedorovTimeout(timeout);
	return dfcamac_wait_lam(_fd, &t);
}



Camac::Fedorov::Crate::Crate(Interface& interface, int idx):
	_interface(interface),
	_idx(idx)
{
	arrayFill(_modules, static_cast<Module*>(0));
	try {
		for (int i = 0; i < arraySize(_modules); i++)
			_modules[i] = new Module(*this, i);
	} catch (...) {
		deleteArrayOfPointers(_modules);
	}
}
Camac::Fedorov::Crate::~Crate() {
	deleteArrayOfPointers(_modules);
}

Camac::Interface & Camac::Fedorov::Crate::interface() const {
	return _interface;
}

Camac::Module * Camac::Fedorov::Crate::getModule(int moduleNumber) {
	if (moduleNumber < 0 || moduleNumber >= arraySize(_modules))
		return 0;
	return _modules[moduleNumber];
}

int Camac::Fedorov::Crate::findFd() const {
	for (int i = 0; i < arraySize(_modules); ++i) {
		if (_modules[i]->_fd < 0)	
			continue;
		return _modules[i]->_fd;
	}
	int rc = _modules[0]->bind();
	if (rc & CAMAC_CC_ERRORS)
		return -(rc & CAMAC_CC_ERRORS);
	assert(_modules[0]->_fd>=0);
	return _modules[0]->_fd;
}
Camac::Result Camac::Fedorov::Crate::C(Timeout  timeout) {
	df_timeout_t t = toFedorovTimeout(timeout);
	int fd = findFd();
	if (fd < 0)
		return -fd;
	return camac_kernel_c(fd, &t);
}

Camac::Result Camac::Fedorov::Crate::Z(Timeout  timeout) {
	df_timeout_t t = toFedorovTimeout(timeout);
	int fd = findFd();
	if (fd < 0)
		return -fd;
	return camac_kernel_z(fd, &t);
}

Camac::Fedorov::Interface::Interface(Camac::Fedorov::Server & server, int idx):
	_server(server),
	_idx(idx)
{
	arrayFill(_crates, static_cast<Crate*>(0));
	try {
		for (int i = 0; i < arraySize(_crates); ++i)
			_crates[i] = new Crate(*this, i);
	}catch(...) {
		deleteArrayOfPointers(_crates);
		throw;
	}
}

Camac::Fedorov::Interface::~Interface() {
	deleteArrayOfPointers(_crates);
}

Camac::Server & Camac::Fedorov::Interface::server() const {
	return _server;
}

Camac::Crate * Camac::Fedorov::Interface::getCrate(int crateIdx) {
	if (crateIdx < 0 || crateIdx > arraySize(_crates))
		return 0;
	return _crates[crateIdx];
}


Camac::Fedorov::Server::Server() {
	arrayFill(_interfaces, static_cast<Interface*>(0));
	try {
		for (int i = 0; i < arraySize(_interfaces); ++i)
			_interfaces[i] = new Interface(*this, i);
	} catch (...) {
		deleteArrayOfPointers(_interfaces);
		throw;
	}
}

Camac::Fedorov::Server::~Server() {
	deleteArrayOfPointers(_interfaces);
}

Camac::Fedorov::Interface * Camac::Fedorov::Server::getInterface(int idx) {
	if (idx < 0 || idx >= arraySize(_interfaces))
		return 0;
	return _interfaces[idx];
}

