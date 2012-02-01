/*
 * Lock_U0301.cpp
 *
 *  Created on: 12.10.2010
 *      Author: gulevich
 */

#include "Lock_U0301.h"
#include <camac/df/std_lcm.h>

static struct camac_lam_check_method Lock_U0301_lcm = { // Not really known. //TODO check and add to constructor
		Lock_U0301::afReadI, 0, CAMAC_LAM_CHECK_BY_Q
};


Lock_U0301::Lock_U0301():
    Base("Lock_U0301", &dfcamac_lcm_none)
{}


int Lock_U0301::Read() {
	return AF(afReadI);
}

int Lock_U0301::Reset() {
	df_timeout_t timeout = 10000; 
	return C(&timeout);
}
