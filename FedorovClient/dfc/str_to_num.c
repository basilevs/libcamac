/*
	<dfc/str_to_num.c>
	$Id: str_to_num.c,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	converts string to signed/unsigned integer and float numbers
	with full check.

	strtonum_cc_t str_to_NUM(const char* src, radix_t base, NUM* dst,
	                         const char **endptrptr);

	Returns condition code of conversion.

	If endptrptr!=NULL *endptrptr will be point to next character
	after converted number in src string.

	If endptrptr==NULL whole string must be appropriate type number or
	Not A Number condition will be returned.

	See also strtol(3).


	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/


#include <dfc/str_to_num.h>


const char* str_strtonum_cc(strtonum_cc_t cc)
{
    switch (cc)
    {
    case strtonumOK:
	return "OK";
    case strtonumOverflow:
	return "Overflow";
    case strtonumUnderflow:
	return "Underflow";
    case strtonumNAN:
	return "Not a Number";
    default:
	return "UNKNOWN code!";
    }
}

