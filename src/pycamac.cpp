#include <boost/python.hpp>

#include "CamacServer.h"
#include "lecroy2249.h"

using namespace boost::python;
using namespace std;

namespace PyCamac {
typedef Camac::Result Result;
typedef Camac::Timeout Timeout;
typedef Camac::AF AF;
typedef uint16_t u16_t;
typedef uint32_t u32_t;

class Error: public std::runtime_error {
	Result _code;
public:
	Error(Result code): runtime_error(""), _code(code) {}
	int code() {return _code;}
	string repr() {
		string rv = "pycamac.Error(";
		bool first = true;
		#define add(x) if (_code & Camac::x) {if (!first) rv+="|"; rv+=#x; first = false;}
		add(CC_OK)
		add(CC_BOOL)
		add(CC_SYSCALL_ERROR)
		add(CC_INVALID_ARG)
		add(CC_NOTBINDED )
		add(CC_NOTINITED )
		add(CC_IFACE_TIMEOUT)
		add(CC_DATA_XFER_ERROR)
		add(CC_VERIFICATION_ERROR)
		add(CC_INTERMEDIATE_ERROR)
		add(CC_TEST_FAILED)
		add(CC_NOT_X)
		add(CC_NOT_Q)
		add(CC_USER_ERROR0 )
		add(CC_USER_ERROR1 )
		add(CC_USER_ERROR2)
		add(CC_USER_ERROR3)
		add(CC_USER_ERRORS)
		#undef add
		rv+=")";
		return rv;
	}
};

class Module {
	Camac::Module & _module;
	Result _ignoredErrors;
	public:
	Module(Camac::Module & module, int ignoredErrors=0): _module(module), _ignoredErrors(ignoredErrors) {}
	void raiseIfError(Result res) {
		if ((res & ~_ignoredErrors) & Camac::CC_ERRORS)
			throw Error(res);
	}
	operator Camac::Module&() {
		return _module;
	}
	void ignoreErrors(int errors) {_ignoredErrors = errors;}
	void af(const AF & afc) {raiseIfError(_module.AFC(afc));}
	void afw16(const AF & af, u16_t data) {raiseIfError(_module.AFW(af, data));}
	void afw24(const AF & af, u32_t data) {raiseIfError(_module.AFW(af, data));}
	u16_t afr16(const AF & af) {
		u16_t rv = 0;	
		raiseIfError(_module.AFR(af, rv));
		return rv;
	}
	u32_t afr24(const AF & af) {
		u32_t rv = 0;	
		raiseIfError(_module.AFR(af, rv));
		return rv;
	}
};

class Crate {
	Camac::Crate & _crate;
	public:
	Crate(Camac::Crate & crate): _crate(crate) {}
	void raiseIfError(Result res) {
		if (res & Camac::CC_ERRORS)
			throw Error(res);
	}
        void C(Timeout  timeout = 0) {
		raiseIfError(_crate.C(timeout));
	}
        void Z(Timeout  timeout = 0) {
		raiseIfError(_crate.Z(timeout));
	}
	Module getModule(int idx) {
		Camac::Module * m = _crate.getModule(idx);
		if (!m)
			throw out_of_range("No module for this index");
		return *m;
	}
};

class Interface {
	Camac::Interface & _interface;
	public:
	Interface(Camac::Interface & interface): _interface(interface) {}
	Crate getCrate(int idx) {
		Camac::Crate * m = _interface.getCrate(idx);
		if (!m)
			throw out_of_range("No crate for this index");
		return *m;
	}
};

class Server {
	public:
	Server() {}
	Interface getInterface(int idx) {
		Camac::Server * s = Camac::Server::getDefault();
		if (!s)
			throw runtime_error("No default server. Relink the module with either cheblakov or fedorov implementation.");
		Camac::Interface * i = s->getInterface(idx);
		if (!i)
			throw out_of_range("No interface with this index");
		return *i;
	}
};

} //namespace PyCamac

//Implementation detail. Only initialized once in module initializer.
static PyObject * errorTypeInstance;
void translateError(PyCamac::Error e)
{
  assert(errorTypeInstance != NULL);
  boost::python::object pythonExceptionInstance(e);
  PyErr_SetObject(errorTypeInstance, pythonExceptionInstance.ptr());
}


BOOST_PYTHON_MODULE(pycamac)
{
	class_<Camac::AF>("Af", init<int, int>());
	class_<PyCamac::Error> CppExceptionWrapper("Error", no_init);
	CppExceptionWrapper.def("__repr__", &PyCamac::Error::repr);
	errorTypeInstance = CppExceptionWrapper.ptr();
	register_exception_translator<PyCamac::Error>(&translateError);
	class_<PyCamac::Module>("Module", no_init)
		.def("afw16", &PyCamac::Module::afw16)
		.def("afw24", &PyCamac::Module::afw24)
		.def("af", &PyCamac::Module::af)
		.def("afr16", &PyCamac::Module::afr16)
		.def("afr24", &PyCamac::Module::afr24)
		.def("ignoreErrors", &PyCamac::Module::ignoreErrors)
;
	class_<PyCamac::Crate>("Crate", no_init)
		.def("c", &PyCamac::Crate::C)
		.def("z", &PyCamac::Crate::Z)
		.def("getModule", &PyCamac::Crate::getModule);
	class_<PyCamac::Interface>("Interface", no_init)
		.def("getCrate", &PyCamac::Interface::getCrate);
	class_<PyCamac::Server>("Server")
		.def("getInterface", &PyCamac::Server::getInterface);

	class_<LeCroy2249>("LeCroy2249", init<Camac::Module&>())
		.def("read", &LeCroy2249::read)
		.def("reset", &LeCroy2249::reset);
}
