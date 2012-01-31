/*
	<dfcommon/_poll_bits.h>
	$Id: _poll_bits.h,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	poll(2) bit mask definitions; helper file for <dfcommon/poll_bits.h>.
	This file should be included in absense of system poll headers only.

	Composed by Dmitry A. Fedorov <dm.fedorov@gmail.com>, 2000
	from GNU C library headers, (C) Free Software Foundation, Inc.
*/

#ifndef  _DFCOMMON__POLL_BITS_H
# define _DFCOMMON__POLL_BITS_H

# ifndef _DFCOMMON_POLL_BITS_H
#  error "Never use <dfcommon/_poll_bits.h> directly;"
#  error "  include <dfcommon/poll_bits.h> instead."
# endif


/* These are specified by iBCS2 */

# define POLLIN     0x0001	/* can be POLLRDNORM | POLLRDBAND */
# define POLLPRI    0x0002
# define POLLOUT    0x0004	/* can be the same value as POLLWRNORM */

# define POLLERR    0x0008
# define POLLHUP    0x0010
# define POLLNVAL   0x0020


/* The rest seem to be more-or-less nonstandard. Check them! */

# define POLLRDNORM 0x0040
# define POLLRDBAND 0x0080	/* CPU dependent, use Linux/i386 */
# define POLLWRNORM 0x0100	/*                               */
# define POLLWRBAND 0x0200	/*                               */

# define POLLMSG    0x0400	/* _USE_GNU */


#endif	/* _DFCOMMON__POLL_BITS_H */

