/*
 * CamacAddressParser.h
 *
 *  Created on: 29.11.2010
 *      Author: gulevich
 */

#ifndef CAMACADDRESSPARSER_H_
#define CAMACADDRESSPARSER_H_

#include <iosfwd>
#include <camac/defs.h>
#include <string>

class CamacAddressParser {
	std::string _driver;
	camac_address _address;
	void updateDriver(const std::string &);
public:
	CamacAddressParser& operator=(const CamacAddressParser &);
	CamacAddressParser();
	CamacAddressParser(const CamacAddressParser &);
	CamacAddressParser(const char *);
	// Parses a string with "/" as separator
	// saves parsing result in this object
	// returns 0 on success, -1 on error
	int parse(const char *);
	const camac_address & address() const {return _address;}
};

std::ostream & operator<<(std::ostream &, const CamacAddressParser&);
bool operator < (const CamacAddressParser & a1, const CamacAddressParser & a2);

#endif /* CAMACADDRESSPARSER_H_ */
