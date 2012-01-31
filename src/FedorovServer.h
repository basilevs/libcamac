#ifndef __FedorovServer_H__
#define  __FedorovServer_H__

#include "CamacServer.h"
#include <vector>
#include <string>

namespace Camac {
namespace Fedorov {
	class Module;
	class Crate;
	class Interface;
	class Server;

	class noncopyable {
		noncopyable & operator=(const noncopyable&);
		noncopyable(const noncopyable&);
	public:
		noncopyable() {}
	};
	class Module: public Camac::Module, private noncopyable {
		Crate & _crate;
		int _idx;
		int _fd;
		friend class Crate;
		Result bind();
	public:
		Module(Crate &, int idx);
		virtual Camac::Crate & crate() const;
		virtual Result AFW(const AF & af, u16_t data);
		virtual Result AFW(const AF & af, u32_t data);
		virtual Result AFR(const AF & af, u16_t & data);
		virtual Result AFR(const AF & af, u32_t & data);
		virtual Result AFC(const AF & af);
		virtual Result WaitLAM(Timeout timeout);
	};
	class Crate: public Camac::Crate, private noncopyable {
		Interface & _interface;
		Module * _modules[26];
		int _idx;
		friend class Module;
	public:
		Crate(Interface&, int idx);
		~Crate();
		int findFd() const;
		virtual Camac::Interface & interface() const;
		virtual Camac::Module * getModule(int moduleNumber);
		virtual Result C(Timeout  timeout);
		virtual Result Z(Timeout  timeout);
	};
	class Interface: public Camac::Interface, private noncopyable {
		Server & _server;
		Crate * _crates[6];
		int _idx;
		friend class Module;
	public:
		Interface(Server&, int idx);
		virtual Camac::Server & server() const;
		virtual Camac::Crate * getCrate(int crateIdx);
		~Interface();
	};
	class Server: public Camac::Server, private noncopyable  {
		Interface * _interfaces[5];
	public:
		Server();
		~Server();
		virtual Interface * getInterface(int interface);
	};
} // namespace Fedorov
} // namespace Camac

#endif
