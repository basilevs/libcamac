/*
	<camac/df/rc2test.c>
	$Id: rc2test.c,v 1.2 2009-12-15 06:08:15 gulevich Exp $

	CAMAC device test helper

	(C) 2000 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

# include <camac/df/rc2test.h>
# include <camac/defs.h>


dev_test_state dfcamac_rc2test(int rc)
{
    dev_test_state dt = DEV_TEST_SUCCESS;

    if (rc<0)
	dt = DEV_TEST_SYSCALL_ERROR;
    else if (rc&CAMAC_CC_IFACE_TIMEOUT)
	dt = DEV_TEST_INTERMEDIATE_ABSENT;
    else if (rc&CAMAC_CC_NOT_X)
	dt = DEV_TEST_ABSENT;
    else if (rc&CAMAC_CC_ERRORS)
	dt = DEV_TEST_BROKEN;

    return dt;
}

