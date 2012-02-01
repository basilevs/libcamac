#ifndef U0302_H__
#define U0302_H__

#include <camac/dfmodule/base.h>

#include <vector>

class U0302: public dfCamacModuleBase {
	float _length;
	std::vector<bool> _enabled;
	int update();
	typedef dfCamacModuleBase Base;
	static const char Type[];
	public:
	U0302();
	enum {
		CHANNEL_COUNT = 6,
		AF_BLOCK = CAMAC_MAKE_AF(0,24),
		AF_UNBLOCK = CAMAC_MAKE_AF(0,24)
	};
	int Turn(int channel, bool on = true);
	int Bind(const camac_address& addr, df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0, int
	flags=0) {
		return Base::Bind(addr, lock_station_timeout,
						flags	| CAMAC_OPEN_STATION_Z_SENSITIVE
								| CAMAC_OPEN_STATION_LOCK_Z );
	}
	int SetLength(float length);
	int Init() {return Base::Init();}
	int Unbind(void){ return Base::Unbind();}
};

#endif
