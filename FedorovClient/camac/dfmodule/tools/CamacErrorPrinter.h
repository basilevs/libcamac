/*
 * ErrorPrinter.h
 *
 *  Created on: 29.11.2010
 *      Author: gulevich
 */

#ifndef CAMACERRORPRINTER_H_
#define CAMACERRORPRINTER_H_

#include <iosfwd>

class CamacErrorPrinter {
	int _cc, _errno;
public:
	int code() const  {return _cc;}
	int getErrno() const  {return _errno;}
	explicit CamacErrorPrinter(int camac_cc_bits);
	operator std::string() const ;
	virtual ~CamacErrorPrinter() {}
};


std::ostream & operator << (std::ostream &, const CamacErrorPrinter &);

#endif /* ERRORPRINTER_H_ */
