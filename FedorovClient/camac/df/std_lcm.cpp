/*
	<camac/df/std_lcm.c>
	$Id: std_lcm.c,v 1.2 2009-12-15 06:08:15 gulevich Exp $

	Most often used LAM check methods

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

# include <camac/df/std_lcm.h>


const struct camac_lam_check_method dfcamac_lcm_none =
{
    CAMAC_INIT_AF(0,0), 0, CAMAC_LAM_CHECK_NONE
};

const struct camac_lam_check_method dfcamac_lcm_a0_f8_q =
{
    CAMAC_INIT_AF(0,8), 0, CAMAC_LAM_CHECK_BY_Q
};

