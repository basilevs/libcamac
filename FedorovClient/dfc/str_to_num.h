/*
	<dfc/str_to_num.h>
	$Id: str_to_num.h,v 1.2 2009-12-15 06:08:16 gulevich Exp $

	converts string to signed/unsigned integer and float numbers
	with full check.

	strtonum_cc_t str_to_NUM(const char* src, radix_t base, NUM* dst,
	                         const char **endptrptr);

	Returns condition code of conversion.

	If endptrptr!=NULL *endptrptr will be point to next character
	after converted number in src string.

	If endptr==NULL whole string must be appropriate type number or
	Not A Number condition will be returned.

	See also strtol(3).


	const char* str_strtonum_cc(strtonum_cc_t cc);

		returns string representation of enum strtonum_cc_t.


	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _DFC_STR_TO_NUM_H
# define _DFC_STR_TO_NUM_H

# include <dfcommon/decls.h>
//# include <dfc/types.h>	/* slong, ulong, radix_t */

__BEGIN_C_DECLS

typedef enum Radix
{
    RADIX_ANY=0,
    RADIX_BINARY=2, RADIX_OCTAL=8, RADIX_DECIMAL=10, RADIX_HEXADECIMAL=16
} radix_t;


/* condition code. all errors is negative */
typedef enum
{
    strtonumOK        =  0,
    strtonumSuccess   =  0,
    strtonumOverflow  = -1,
    strtonumUnderflow = -2,
    strtonumNAN       = -3
} strtonum_cc_t;


strtonum_cc_t str_to_slong    (const char* src, radix_t base, long    * dst,
			       const char **endptrptr);

strtonum_cc_t str_to_slonglong(const char* src, radix_t base, long long* dst,
			       const char **endptrptr);

strtonum_cc_t str_to_ulong    (const char* src, radix_t base, unsigned long    * dst,
			       const char **endptrptr);

strtonum_cc_t str_to_ulonglong(const char* src, radix_t base, unsigned long long* dst,
			       const char **endptrptr);

strtonum_cc_t str_to_float    (const char* src, radix_t unused,  double* dst,
			       const char **endptrptr);

const char*   str_strtonum_cc (strtonum_cc_t cc);


__END_C_DECLS

#endif	/* _DFC_STR_TO_NUM_H */

