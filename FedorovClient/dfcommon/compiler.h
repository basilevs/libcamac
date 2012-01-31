/*
	<dfcommon/compiler.h>
	$Id: compiler.h,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	(C) 2003 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _DFCOMMON_COMPILER_H
# define _DFCOMMON_COMPILER_H


# ifdef __GNUC__

#  ifndef  __GNUC_PATCHLEVEL__
#   define __GNUC_PATCHLEVEL__ 0
#  endif

#  ifndef  GCC_VERSION
#   define GCC_VERSION(major, minor, patchlevel)	\
	((major << 16) + (minor << 8) + patchlevel)
#  endif

#  ifndef  GCC_VERSION_CODE
#   define GCC_VERSION_CODE	\
           GCC_VERSION(__GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__)
#  endif

# endif /* def __GNUC__ */


#endif	/* _DFCOMMON_COMPILER_H */

