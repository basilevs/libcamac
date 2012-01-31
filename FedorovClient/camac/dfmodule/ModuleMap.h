/*
 * ModuleMap.h
 *
 *  Created on: 23.11.2010
 *      Author: gulevich
 */

#ifndef MODULEMAP_H_
#define MODULEMAP_H_
#include "base.h"
#include <assert.h>
#include <map>

class ModuleMap: private std::map<int, Camac::Module *> {
	int _last;
public:
	typedef std::map<int, Camac::Module * > Base;
	ModuleMap():_last(0) {}
	Camac::Module * get(int fd) {
		assert(fd >= 0);
		Camac::Module * rv = operator[](fd);
		if (!rv)
			erase(fd);
		return rv;
	}
	int add(Camac::Module & module) {
		int newFd = ++_last;
		std::pair<iterator, bool> rv = insert(std::make_pair(newFd, &module));
		assert(rv.second && "Fd overflow");
		return newFd;
	}
	using Base::erase;
};

#endif /* MODULEMAP_H_ */
