/*
	<camac/dfmodule/base.h>
	$Id: base.h,v 1.2 2009-12-15 06:08:15 gulevich Exp $

	CAMAC module base class

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _CAMAC_DFMODULE_BASE_H
# define _CAMAC_DFMODULE_BASE_H

//# include <camac/kernel.h>
//# include <camac/bad.h>

#include <camac/defs.h>
#include <vector>
#include <limits>


namespace Camac {
	class Module;
}
class dfCamacModuleBase
{
	Camac::Module * _module;
	typedef std::vector<camac_af_list> AfList;
	AfList _onCloseAfList;
    const struct camac_lam_check_method* _lcm;	//the same or 0
    int _station;
protected:
    dfCamacModuleBase(const char* _type, const camac_lam_check_method* _lcm,
		      const char* _signature=0, void* _shmptr=0,
		      size_t _shmsize=0 );

    ~dfCamacModuleBase();


    int Bind(const camac_address& _addr,
	     df_timeout_t* lock_station_timeout = DF_TIMEOUT_PTR_0,
	     int flags=0);

    //return cc_bits, unbinds anyway, see close(2)
    int Unbind(void);


    int Lock(int , df_timeout_t* =DF_TIMEOUT_PTR_INF)
    { return 0;}

    int Unlock(int )
    { return 0;}


    int LockStation(df_timeout_t* =DF_TIMEOUT_PTR_INF)
    { return 0; }

    int UnlockStation(void)
    { return 0; }

protected:
    int Init  (void) { return 0; }

    int Verify(void) { return 0; }

    dev_test_state Test(int  = -1)
    { return DEV_TEST_SUCCESS; }

protected:
    int AF(camac_af_t af, u16_t* data);

    int AF(camac_af_t af, u32_t* data);

    int AF(camac_af_t af);

//    int BlockAF(camac_af_t af, struct camac_block16* block);
//    int BlockAF(camac_af_t af, struct camac_block24* block);

    int ListAF(camac_af_list* aflist, size_t count);

//    int SetAfListOnClose(const camac_af_list* aflist, size_t count)
//    { return camac_kernel_set_aflist_on_close(stfd, aflist, count); }

public:
    int C(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF);

    int Z(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF);

    int   LockC(void)	{ return CAMAC_CC_OK; }
    int   LockZ(void)	{ return CAMAC_CC_OK; }

    int UnlockC(void)	{ return CAMAC_CC_OK; }
    int UnlockZ(void)	{ return CAMAC_CC_OK; }
    int WaitLAM(df_timeout_t* timeout);

    int   SetAndLockI(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF);

    int ClearAndLockI(df_timeout_t* timeout=DF_TIMEOUT_PTR_INF);

    int UnlockI(void)
    { return CAMAC_CC_OK; }

//    int GetI    (void)	{ return camac_kernel_get_i     (stfd); }
//    int GetIphys(void)	{ return camac_kernel_get_i_phys(stfd); }

protected:
    int CompleteLAM(void) {return CAMAC_CC_OK;}
//    { return camac_kernel_complete_lam(stfd); }

    int SetEventsMask(ushort ) {return CAMAC_CC_OK;}
    int GetEventsMask(void) const {return 0;}
    int GetEvents    (void) const {return 0;}

protected:
    //returns cc bits with NOT_Q inverted.
    static inline int InverseNotQ(int cc)
    { return (cc & ~CAMAC_CC_NOT_Q) | ((~cc) & CAMAC_CC_NOT_Q); }

public:
//    const camac_address& GetAddr(void) const { return addr; }

//    const char* GetDrvName(void) const { return addr.driver_name; }
//    camac_i_t   GetIface  (void) const { return addr.iface;       }
//    camac_c_t   GetChannel(void) const { return addr.crate;       }
    //This is for debugging gCamac modules.
	camac_n_t   GetSlot   (void) const { return _station;     }
};

//Information about event handler
class LAMHandler {
public:
	typedef void Callback(void*);
	//Note, that the module should be binded before constructing this handler.
	LAMHandler(dfCamacModuleBase & module, Callback, void * data);
	~LAMHandler();
private:
	static void * staticCallback(void *);
	dfCamacModuleBase & _module;
	Callback * _cb;
	void * _data;
	unsigned long _thread;
	volatile bool exitFlag;
};

#endif	/* _CAMAC_DFMODULE_BASE_H */

