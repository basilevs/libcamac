/*
 * MamkinServer.cpp
 *
 *  Created on: 14.09.2010
 *      Author: gulevich
 */

#include "MamkinServer.h"
#include "lsi6camac.h"
#include <utility>
#include <iostream>
#include <sstream>
#include <assert.h>
#include <unistd.h>

#include "actools.h"

#include <cmath>

using namespace Camac;
using namespace std;

#ifndef MULTIPLE_CAMAC_SERVERS
static Mamkin::Server mamServer;
Server * Server::_default = &mamServer;
#endif

Result txqToResultBits(int xq) {
	Result rv =	CC_OK;
	rv |= (xq & 4 ? CC_IFACE_TIMEOUT : CC_OK);
	rv |= (xq & 2 ? CC_NOT_X : CC_OK);
	rv |= (xq & 1 ? CC_NOT_Q : CC_OK);
	rv |= (xq < 0 ? CC_DATA_XFER_ERROR : CC_OK);
	return rv;
}

#define GETFD int f = fd(); if (f<0) return -f;

Result Mamkin::Module::waitLAM(Timeout timeout) {
	int mamTime = 1;
	if (isinf(timeout)) {
		mamTime = -1;
	} else if (timeout <= 0) {
		mamTime = 0;
	} else {
		mamTime = int(timeout*100);
	}
	GETFD
	mamTime = CAM_LWAIT(f, _idx+1, mamTime);
	if (mamTime < 0) {
		return CC_OK;
	} else {
		return CC_BOOL;
	}
}

Crate & Mamkin::Module::crate() const {return  _crate;}
int Mamkin::Module::fd() {
	return _crate.fd();
}
	
Result Mamkin::Module::AFR(const struct AF & af, u16_t & data) {
	if (!af.isValid())
		return CC_INVALID_ARG;
	if (!af.isRead())
		return CC_INVALID_ARG;
	unsigned dummy = data;
	GETFD
	int xq = CAM(f, NAF(_idx+1, af.a, af.f), &dummy);
	data = dummy;
	return txqToResultBits(xq);
}

Result Mamkin::Module::AFR(const struct AF & af, u32_t & data) {
	if (!af.isValid())
		return CC_INVALID_ARG;
	if (!af.isRead())
		return CC_INVALID_ARG;
	unsigned dummy = data;
	GETFD
	int xq = CAM(f, NAF24(_idx+1, af.a, af.f), &dummy);
	data = dummy;
	return txqToResultBits(xq);
}

Result Mamkin::Module::AFW(const struct AF & af, u16_t  data) {
	if (!af.isValid())
		return CC_INVALID_ARG;
	if (!af.isWrite())
		return CC_INVALID_ARG;
	GETFD
	int xq = CAMW(fd(), NAF(_idx+1, af.a, af.f), data);
	return txqToResultBits(xq);
}

Result Mamkin::Module::AFW(const struct AF & af, u32_t  data) {
	if (!af.isValid())
		return CC_INVALID_ARG;
	if (!af.isWrite())
		return CC_INVALID_ARG;
	GETFD
	int xq = CAM24W(f, NAF24(_idx+1, af.a, af.f), data);
	return txqToResultBits(xq);
}

Result Mamkin::Module::AFC(const struct AF & af) {
	if (!af.isValid())
		return CC_INVALID_ARG;
	GETFD
	int xq = CAM(f, NAF(_idx+1, af.a, af.f), 0);
	return txqToResultBits(xq);
}

Mamkin::Module::Module(Mamkin::Crate & crate, int idx): _crate(crate), _idx(idx) {}


int Mamkin::Crate::fd() {
	if (_fd < 0) {
		ostringstream path;
		path << "/dev/lsi6card" << _interface._idx+1 << "channel" << _idx+1;
		if (access(path.str().c_str(), W_OK)!=0) {
			return -CC_USER_ERROR0;
		}
		_fd = open(path.str().c_str(),O_RDWR);
		if (_fd < 0) {
			cerr << "Unable to open " << path.str() << " fd = " << _fd << endl;
		}
	}
	if (_fd < 0)
		_fd = -CC_NOTBINDED;
	return _fd;
}
	
Mamkin::Crate::Crate(Mamkin::Interface & interface, int idx):
	_interface(interface),
	_idx(idx),
	_fd(-1)
{
	fillArrayOfPointersWithIndexed(_modules, *this);
}
Mamkin::Crate::~Crate() {
	deleteArrayOfPointers(_modules);
}

Result Mamkin::Crate::C(Timeout timeout) {
	GETFD
	return CAM_C(f) ? CC_OK : CC_DATA_XFER_ERROR;
}

Result Mamkin::Crate::Z(Timeout timeout) {
	GETFD
	return CAM_Z(f) ? CC_OK : CC_DATA_XFER_ERROR;
}

Interface & Mamkin::Crate::interface() const {return _interface;}
Module * Mamkin::Crate::getModule(int idx) {
	return getArrayItemOrNull(_modules, idx);
}




Mamkin::Interface::Interface(Mamkin::Server & server, int idx): _server(server), _idx(idx) {
	fillArrayOfPointersWithIndexed(_crates, *this);
}

Mamkin::Interface::~Interface() {
	deleteArrayOfPointers(_crates);
}

Crate * Mamkin::Interface::getCrate(int idx) {
	Mamkin::Crate *crate = getArrayItemOrNull(_crates, idx);
	if (!crate)
		return 0;
	if (crate->fd() < 0)
		return 0;
	return crate;
}

Server & Mamkin::Interface::server() const {return _server;}

Mamkin::Server::Server() {
	fillArrayOfPointersWithIndexed(_interfaces, *this);
}
Mamkin::Server::~Server() {
	deleteArrayOfPointers(_interfaces);
}
Interface * Mamkin::Server::getInterface(int idx) {
	return getArrayItemOrNull(_interfaces, idx);
}


