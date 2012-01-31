/*
 * CamacServer.h
 *
 *  Created on: 14.09.2010
 *      Author: gulevich
 */

#ifndef CAMACSERVER_H_
#define CAMACSERVER_H_

#include <stdint.h>
#include <iosfwd>

namespace Camac {

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;


// stores timeout in seconds
// INF means no timeout
// negative values mean immediate exit
typedef float Timeout;
typedef int Result;

/*
struct Address {
	int iface, crate, station;
	Address() {reset();}
	void reset() {iface=crate=station=-1;}
};

struct Channel {
	int iface, crate;
	Channel(const Address & addr):
		iface(addr.iface),
		crate(addr.crate)
	{}
	bool operator < (const Channel & o) const {
		if (iface != o.iface) return iface < o.iface;
		return crate < o.crate;
	}
};
*/



struct AF {
	int a, f;
	AF(): a(-1), f(-1) {}
	AF(int _a, int _f): a(_a), f(_f) {}
	bool isRead() const {return 0 <= f && f <= 7;}
	bool isWrite() const {return 16 <= f && f <= 23;}
	bool isControl() const {return (8 <= f && f <=15) || (8 <= 24 && f <= 31);}
	bool isValid() const {return 0 <= f && f <= 31 && 0 <= a && a <= 15;}
};

enum	/* camac_condition_bit_shifts */
{
    /* additional logical answer */
    BOOL_SHIFT               =  0,

    /* software errors */
    SYSCALL_ERROR_SHIFT      =  1,
    INVALID_ARG_SHIFT        =  2,
    NOTBINDED_SHIFT          =  3,
    NOTINITED_SHIFT          =  4,

    /* hardware errors */
    IFACE_TIMEOUT_SHIFT      =  5, /* no reply             */
    DATA_XFER_ERROR_SHIFT    =  6, /* data transfer error  */
    VERIFICATION_ERROR_SHIFT =  7, /* too long to describe */
    INTERMEDIATE_ERROR_SHIFT =  8, /* intermediate device error */
    TEST_FAILED_SHIFT        =  9, /* last test was failed */

    /* Station answer.                                        */
    /* These bits can be returned by station operations only. */
    NOT_X_SHIFT              = 10,
    NOT_Q_SHIFT              = 11,

    USER_ERROR0_SHIFT        = 12,
    USER_ERROR1_SHIFT        = 13,
    USER_ERROR2_SHIFT        = 14,
    USER_ERROR3_SHIFT        = 15,
};

enum ResultBits {
	CC_OK = 0,
	/* additional logical answer */
	CC_BOOL               = 1<<BOOL_SHIFT,

	/* software errors */
	CC_SYSCALL_ERROR      = 1<<SYSCALL_ERROR_SHIFT,
	CC_INVALID_ARG        = 1<<INVALID_ARG_SHIFT  ,
	CC_NOTBINDED          = 1<<NOTBINDED_SHIFT    ,
	CC_NOTINITED          = 1<<NOTINITED_SHIFT    ,

	CC_SOFT_ERRORS      =
		  CC_SYSCALL_ERROR
		| CC_INVALID_ARG
		| CC_NOTBINDED
		| CC_NOTINITED,


	/* hardware errors */
	CC_IFACE_TIMEOUT      = 1<<IFACE_TIMEOUT_SHIFT     ,
	CC_DATA_XFER_ERROR    = 1<<DATA_XFER_ERROR_SHIFT   ,
	CC_VERIFICATION_ERROR = 1<<VERIFICATION_ERROR_SHIFT,
	CC_INTERMEDIATE_ERROR = 1<<INTERMEDIATE_ERROR_SHIFT,
	CC_TEST_FAILED        = 1<<TEST_FAILED_SHIFT,

	CC_HARD_ERRORS      =
		  CC_IFACE_TIMEOUT
		| CC_DATA_XFER_ERROR
		| CC_VERIFICATION_ERROR
		| CC_INTERMEDIATE_ERROR
		| CC_TEST_FAILED,


	CC_ERRORS_ONLY        =
		  CC_SOFT_ERRORS
		| CC_HARD_ERRORS,


	/* Station answer.                                        */
	/* These bits may be returned by station operations only. */
	CC_NOT_X              = 1<<NOT_X_SHIFT,
	CC_NOT_Q              = 1<<NOT_Q_SHIFT,
	CC_NOT_XQ             = CC_NOT_X | CC_NOT_Q,

	/* User errors extension */
	CC_USER_ERROR0        = 1<<USER_ERROR0_SHIFT,
	CC_USER_ERROR1        = 1<<USER_ERROR1_SHIFT,
	CC_USER_ERROR2        = 1<<USER_ERROR2_SHIFT,
	CC_USER_ERROR3        = 1<<USER_ERROR3_SHIFT,

	CC_USER_ERRORS        =
		  CC_USER_ERROR0 | CC_USER_ERROR1
		| CC_USER_ERROR2 | CC_USER_ERROR3,

	/* error for user - errors and station answer */
	CC_ERRORS             =
		CC_ERRORS_ONLY | CC_NOT_XQ | CC_USER_ERRORS,

	CC_ALL                =
		CC_ERRORS | CC_BOOL,

	/* this enum must has size of int */
	__INT              = -1
};

struct Module;
struct Crate;
struct Server;
struct Interface;

class Module {
public:
	virtual Crate & crate() const = 0;
	virtual Result AFW(const AF & af, u16_t data) = 0;
	virtual Result AFW(const AF & af, u32_t data) = 0;
	virtual Result AFR(const AF & af, u16_t & data) = 0;
	virtual Result AFR(const AF & af, u32_t & data) = 0;
	virtual Result AFC(const AF & af) = 0;
	virtual Result waitLAM(Timeout timeout = 0) = 0;
	virtual ~Module(){}
};

class Crate {
public:
	virtual Interface & interface() const = 0;
	virtual Module * getModule(int moduleNumber) = 0;
	virtual Result C(Timeout  timeout = 0) = 0;
	virtual Result Z(Timeout  timeout = 0) = 0;
	virtual ~Crate() {}
};

class Interface {
public:
	virtual Server & server() const =0;
	virtual Crate * getCrate(int crateIdx) =0;
	virtual ~Interface() {}
};

class Server {
	static Server * _default;
public:
	virtual Interface * getInterface(int interface) =0;
	static Server * getDefault() {return _default;}
	virtual ~Server() {}
};

std::ostream & printResult(std::ostream& o, Result res);
std::string resultToString(Result);

Module & getModule(Server *, int interface, int crate, int module);

};
#endif /* CAMACSERVER_H_ */
