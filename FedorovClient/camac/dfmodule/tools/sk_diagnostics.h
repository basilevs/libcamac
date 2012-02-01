
#ifndef  SK_PRINT_ERRORS_H
#define SK_PRINT_ERRORS_H

#include <camac/defs.h>
#include <dfcommon/dev_test_state.h>

#define FC_DEFAULT "\033[0m"
#define FC_STANDOUT "\033[0;1m"
#define FC_OK "\033[0;32;1m"
#define FC_HEADER "\033[0;44;37;1m"
#define FC_WARNING "\033[0;33;1m\a"
#define FC_ERROR "\033[0;31;1m\a"
#define FC_FATALERROR "\033[0;41;37;1m\a"

#ifdef  __cplusplus
extern "C" {
#endif

extern int sk_diagnose(int status, int rc, const char* format, ...);
extern void sk_error(int status, int errnum, const char* format, ...);

extern void sk_print_header(FILE* stream, const char* str, ...);
extern void sk_print_standout(FILE* stream, const char* str, ...);
extern void sk_print_warning(FILE* stream, const char* str, ...);
extern void sk_print_header_endl(FILE* stream, const char* str, ...);
extern void sk_print_standout_endl(FILE* stream, const char* str, ...);
extern void sk_print_warning_endl(FILE* stream, const char* str, ...);

inline int sk_bind_diagnose(int status, int rc, const char* name, const struct camac_address addr)
{
	return sk_diagnose(status,rc,"%s %s[%i][%i][%i] bind",name,
		addr.driver_name,addr.iface,addr.crate,addr.station);
}
inline int sk_unbind_diagnose(int status, int rc, const char* name)
{
	return sk_diagnose(status,rc,"%s unbind",name);
}

inline int sk_init_diagnose(int status, int rc, const char* name)
{
	return sk_diagnose(status,rc,"%s init",name);
}

inline int sk_verify_diagnose(int status, int rc, const char* name)
{
	return sk_diagnose(status,rc,"%s verify",name);
}

extern dev_test_state sk_test_diagnose(int status, dev_test_state dt, const char* name);

extern int sk_silent_ok;

#ifdef  __cplusplus
}
#endif

#endif	//SK_PRINT_ERRORS_H

