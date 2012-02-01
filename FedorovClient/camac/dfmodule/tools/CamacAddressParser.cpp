/*
 * CamacAddressParser.cpp
 *
 *  Created on: 29.11.2010
 *      Author: gulevich
 */

#include "CamacAddressParser.h"
#include <string.h>
#include <iostream>
#include <sstream>
//#include <camac/df/parse_address.h>


using namespace std;


void CamacAddressParser::updateDriver(const string & name) {
	_driver = name;
	_address.driver_name = const_cast<char*>(_driver.c_str());
}

static void init(camac_address & a) {
	a = {0};
}
CamacAddressParser::CamacAddressParser() {
	init(_address);
}
CamacAddressParser::CamacAddressParser(const char * input)
{
	init(_address);
	parse(input);
}

CamacAddressParser& CamacAddressParser::operator=(const CamacAddressParser & that) {
	_address = that._address;
	updateDriver(that._driver);
	return *this;
}


CamacAddressParser::CamacAddressParser(const CamacAddressParser & that)
{
	*this = that;
}

int CamacAddressParser::parse(const char * input) {
	if (!input)
		return -1;
	size_t len = strlen(input);
	_address.driver_name = 0;
	_address.station = _address.iface = _address.crate = 255;
	istringstream s(input);
	string temp;
	int number;
	char delim = 0;
	getline(s, _driver, '/');
	s >> number >> delim;
	_address.iface = number;
	if (delim != '/')
		s.setstate(ios::failbit);
	s >> number >> delim;
	_address.crate = number;
	if (delim != '/')
		s.setstate(ios::failbit);
	s >> number;
	_address.station = number;
	if (!s) {
		cerr << "address " << input << " is incorrect " << endl;
		return -1;
	}
	updateDriver(_driver);
	return 0;
}

ostream & operator << (ostream & ostr, const CamacAddressParser & p) {
	const camac_address & a = p.address(); 
	return ostr << a.driver_name << "/" << int(a.iface) << "/" << 
int(a.crate) << "/"<< int(a.station);
}

bool operator < (const CamacAddressParser & a1, const CamacAddressParser & a2) {
	const camac_address & v1 = a1.address(), & v2 = a2.address();
	int res = 0;
	if (v1.driver_name && v2.driver_name) {
		strcmp(v1.driver_name, v2.driver_name);
	} else {
		if (!v1.driver_name)
			return true;
		return false;
	}
	if (res != 0)
		return res < 0;
	if (v1.iface != v2.iface)
		return v1.iface < v2.iface;
	if (v1.crate != v2.crate)
		return v1.crate < v2.crate;
	if (v1.station != v2.station)
		return v1.station < v2.station;
	return false;
}
