
#ifndef  _bmyu_out_err_h
# define _bmyu_out_err_h

#include <camac/defs.h>
//#include <dfcommon/dev_test_state.h>

int out_err(const char* str, int rc);

int out_bind_err(const char* name, const camac_address& addr, int rc);

int out_unbind_err(const char* name, int rc);

int out_init_err(const char* name, int rc);

//dev_test_state out_test_err(const char* name, dev_test_state dt);

int out_verify_err(const char* name, int rc);

#endif	//_bmyu_out_err_h

