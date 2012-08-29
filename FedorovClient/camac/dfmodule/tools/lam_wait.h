#ifndef LAM_WAIT_H_
#define LAM_WAIT_H_

#include <camac/df/wait_lam.h>
#include "base.h"
#include <errno.h>

template<class T>
int lam_wait(T & module, df_timeout_t * timeout) {
#ifdef CAMAC_MODULE_MOCK
	return module.WaitLAM(timeout);
#else
	camac_kernel_complete_lam(module.GetFD());
	int rv = dfcamac_wait_lam(module.GetFD(), timeout);
	if (rv == -1 &&  errno == ETIME)
		return 0;
	return rv;
#endif
}


/*
template<class T>
int lam_complete(T & module) {
#ifndef CAMAC_MODULE_MOCK
	int rv = (module.GetFD(), timeout);
	if (rv == -1 &&  errno == ETIME)
		return 0;
	return rv;
#endif
}
*/
#endif /* LAM_WAIT_H_ */
