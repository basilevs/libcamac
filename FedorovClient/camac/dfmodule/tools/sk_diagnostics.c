#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#include <camac/df/print_errors.h>
#include <dfc/print_devtest.h>
#include <camac/df/rc2test.h>

#include "sk_diagnostics.h"

int sk_silent_ok = 0; //report on success by default

void sk_print_header(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_HEADER);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT);
	fflush(stream);
	va_end(ap);
}
void sk_print_header_endl(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_HEADER);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT "\n");
	fflush(stream);
	va_end(ap);
}
void sk_print_standout(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_STANDOUT);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT);
	fflush(stream);
	va_end(ap);
}
void sk_print_standout_endl(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_STANDOUT);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT "\n");
	fflush(stream);
	va_end(ap);
}
void sk_print_warning(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_STANDOUT);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT);
	fflush(stream);
	va_end(ap);
}
void sk_print_warning_endl(FILE* stream, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);
    fprintf(stream,FC_STANDOUT);
	vfprintf(stream,format,ap);
	fprintf(stream,FC_DEFAULT "\n");
	fflush(stream);
	va_end(ap);
}

int sk_diagnose(int status, int rc, const char* format, ...)
{
	va_list ap;

	if( sk_silent_ok && !(rc&CAMAC_CC_ERRORS) ) return rc;

	va_start(ap, format);

	if( rc&CAMAC_CC_ERRORS ) {
		if( status!=0 )
			fprintf(stderr,FC_FATALERROR);
		else
			fprintf(stderr,FC_ERROR);
	}
    else
		fprintf(stderr,FC_OK);

	vfprintf(stderr, format, ap);

	va_end(ap);

	fprintf(stderr,": ");
	if( rc<0 )
		fprintf(stderr,strerror(errno));
	else {
		if( rc&CAMAC_CC_ERRORS )
			dfcamac_print_errors(stderr,rc);
		else
            fprintf(stderr,"OK");
	}

	fprintf(stderr,FC_DEFAULT "\n");

	if( status!=0 && rc&CAMAC_CC_ERRORS )
		exit(status);

	return rc;
}

void sk_error(int status, int errnum, const char* format, ...)
{
	va_list ap;

	va_start(ap, format);

	if( status!=0 )
		fprintf(stderr,FC_FATALERROR);
	else
		fprintf(stderr,FC_ERROR);

	vfprintf(stderr, format, ap);

	va_end(ap);

	if( errnum )
		fprintf(stderr,": %s",strerror(errno));

	fprintf(stderr,FC_DEFAULT "\n");

	if( status!=0 )
		exit(status);
}

dev_test_state sk_test_diagnose(int status, dev_test_state dt, const char* name)
{
	if( sk_silent_ok && dt==DEV_TEST_OK ) return dt;

	if( dt!=DEV_TEST_OK ) {
		if( status!=0 )
			fprintf(stderr,FC_FATALERROR);
		else
			fprintf(stderr,FC_ERROR);
	}
	else
		fprintf(stderr,FC_OK);

	fprintf(stderr,"%s test: ", name);

	if( dt!=DEV_TEST_OK )
		df_print_devtest(stderr,dt);
	else
		fprintf(stderr,"OK");

	fprintf(stderr,FC_DEFAULT "\n");

	if( status!=0 && dt!=DEV_TEST_OK )
		exit(status);

	return dt;
}

