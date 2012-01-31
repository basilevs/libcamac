/*
	<dfcommon/poll_bits.h>
	$Id: poll_bits.h,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	poll(2) bit mask definitions.

	(C) 2000 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _DFCOMMON_POLL_BITS_H
# define _DFCOMMON_POLL_BITS_H

# ifdef linux

#  ifdef __KERNEL__

#   include <linux/compat/version.h> /* LINUX_VERSION_CODE, KERNEL_VERSION() */

#   if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,23)
#    include <asm/poll.h>
#   else
#    include <dfcommon/_poll_bits.h>
#   endif

#  else		/* !__KERNEL__ */

#   include <features.h>

#   if defined(__GLIBC__) && __GLIBC__ >= 2 && __GLIBC_MINOR__ >= 1
#    include <bits/posix_opt.h>
#   else
#    include <posix_opt.h>
#   endif

#   ifdef _POSIX_POLL
#    include <sys/poll.h>
#   else
#    include <dfcommon/_poll_bits.h>
#   endif

#  endif	/* __KERNEL__ */
# else		/* linux      */
#  error not supported
# endif

#endif	/* _DFCOMMON_POLL_BITS_H */

