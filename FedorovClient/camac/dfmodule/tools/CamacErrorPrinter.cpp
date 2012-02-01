/*
 * ErrorPrinter.cpp
 *
 *  Created on: 29.11.2010
 *      Author: gulevich
 */

#include "CamacErrorPrinter.h"
#include <sstream>
#include <camac/defs.h>
#include <errno.h>


CamacErrorPrinter::CamacErrorPrinter(int camac_cc_bits): _cc(camac_cc_bits), _errno(errno){}

#define TEST_BIT(a) if (CAMAC_CC_##a & cc) {o << #a << " "; cc &= ~CAMAC_CC_##a;}
std::ostream & operator << (std::ostream & o , const CamacErrorPrinter & p) {
	int cc = p.code();
    if (p.code() == -1 && p.getErrno()==ETIME) {
    	return o << "ETIME";
    }
		if (p.code() == -1) {
			return o << "UNKNOWN";
		}
   	TEST_BIT(BOOL);
    TEST_BIT(SYSCALL_ERROR);
    TEST_BIT(INVALID_ARG);
    TEST_BIT(NOTBINDED);
    TEST_BIT(NOTINITED);

    TEST_BIT(IFACE_TIMEOUT);
    TEST_BIT(DATA_XFER_ERROR);
    TEST_BIT(VERIFICATION_ERROR);
    TEST_BIT(INTERMEDIATE_ERROR);
    TEST_BIT(TEST_FAILED);

    TEST_BIT(NOT_X);
    TEST_BIT(NOT_Q);

    TEST_BIT(USER_ERROR0);
    TEST_BIT(USER_ERROR1);
    TEST_BIT(USER_ERROR2);
    TEST_BIT(USER_ERROR3);
    
    return o;
};

CamacErrorPrinter::operator std::string() const {
	std::ostringstream str;
	str << this;
	return str.str();
}

