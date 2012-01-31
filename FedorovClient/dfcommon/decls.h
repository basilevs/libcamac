/*
	<dfcommon/decls.h>
	$Id: decls.h,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	some declaration macros

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _DFCOMMON_DECLS_H
# define _DFCOMMON_DECLS_H

# include <dfcommon/compiler.h>


# ifdef	__cplusplus
#  define	__C_DECL extern "C"
#  define	__BEGIN_C_DECLS	__C_DECL {
#  define	__END_C_DECLS	}
# else
#  define	__C_DECL
#  define	__BEGIN_C_DECLS
#  define	__END_C_DECLS
# endif


/* For C++ programs we add throw() to help it optimize the function calls. */
/* But this works only with gcc 2.8.x and egcs.                            */

# ifndef   __NOTHROW
#  if defined __GNUG__ && GCC_VERSION_CODE >= GCC_VERSION(2,8,0)
#   define __NOTHROW	throw ()
#  else
#   define __NOTHROW
#  endif
# endif	// ndef __NOTHROW


#endif	/* _DFCOMMON_DECLS_H */

