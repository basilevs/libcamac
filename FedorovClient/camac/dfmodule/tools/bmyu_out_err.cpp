#include <stdio.h>
#include <stdlib.h>

//#include <dfcompat/error.h>
//#include <dfcompat/errno.h>
#include <string.h>
#include <errno.h>
#include "bmyu_out_err.h"


#include "CamacErrorPrinter.h"
#include <string>
#include <stdio.h>
const char
    str_dev_test_Success                [] = "Success",

    str_dev_test_NotBinded              [] = "Not binded",
    str_dev_test_Absent                 [] = "Absent",
    str_dev_test_Broken                 [] = "Broken",
    str_dev_test_Incomplete             [] = "Incomplete",

    str_dev_test_IntermediateNotBinded  [] = "Intermediate device is not binded",
    str_dev_test_IntermediateAbsent     [] = "Intermediate device is absent",
    str_dev_test_IntermediateBroken     [] = "Intermediate device is broken",
    str_dev_test_IntermediateIncomplete [] = "Intermediate device is incomplete",

    str_dev_test_SyscallError           [] = "syscall error",
    str_dev_test_Unknown                [] = "Unknown"
;


const char* strdevtest(dev_test_state dt)
{
    if (dt<0)
        return strerror(errno);

    switch (dt)
    {
    case DEV_TEST_SUCCESS:
         return str_dev_test_Success;

     case DEV_TEST_NOTBINDED:
         return str_dev_test_NotBinded;
     case DEV_TEST_ABSENT:
         return str_dev_test_Absent;
     case DEV_TEST_BROKEN:
         return str_dev_test_Broken;
     case DEV_TEST_INCOMPLETE:
         return str_dev_test_Incomplete;

     case DEV_TEST_INTERMEDIATE_NOTBINDED:
         return str_dev_test_IntermediateNotBinded;
     case DEV_TEST_INTERMEDIATE_ABSENT:
         return str_dev_test_IntermediateAbsent;
     case DEV_TEST_INTERMEDIATE_BROKEN:
         return str_dev_test_IntermediateBroken;
     case DEV_TEST_INTERMEDIATE_INCOMPLETE:
         return str_dev_test_IntermediateIncomplete;

     case DEV_TEST_SYSCALL_ERROR:
         return str_dev_test_SyscallError;

     default:
         break;
     }

     return str_dev_test_Unknown;
 }

void df_print_devtest(FILE* f, dev_test_state dt)
{
    const char* str=strdevtest(dt);

    if (dt<0)   /* syscall error */
        fprintf(f, "%s: %s", str, strerror(errno) );
    else
        fprintf(f, "%s", str);
}


inline size_t dfcamac_print_errors     (FILE * stream, int rc) {
	CamacErrorPrinter err(rc);
	fprintf(stream, std::string(err).c_str());
}

inline size_t dfcamac_print_errors_endl(FILE * stream, int rc) {
	CamacErrorPrinter err(rc);
	fprintf(stream, (std::string(err)+"\n").c_str());
}


int out_err(const char* str, int rc)
{
	if( rc & CAMAC_CC_ERRORS )
		if( rc<0 )
		{
			fprintf(stderr,"\07%s error",str);
			perror(" ");
		}
		else
		{
			fprintf(stderr,"\07%s CAMAC error:",str);
			dfcamac_print_errors(stderr,rc);
		}

	return rc;
}

int out_bind_err(const char* name, const camac_address& addr, int rc)
{
	if( rc )
	{
		fprintf(stderr,"%s\t%s[%i][%i][%2i] bind :", name,
			addr.driver_name, addr.iface, addr.crate, addr.station);
		fprintf(stderr," FD=%i",rc);
		perror(" ");
	}
//	else
//		fputs(" Success...\n",stderr);

	return rc;
}

int out_unbind_err(const char* name, int rc)
{
	if(rc)
	{
		fprintf(stderr,"%s\t unbind :", name);
		if (rc&CAMAC_CC_ERRORS)
			dfcamac_print_errors(stderr,rc);
		else
			perror(" ");
	}
//	else
//		fputs(" Success...\n",stderr);

	return rc;
}

int out_init_err(const char* name, int rc)
{
	if(rc)
	{
		fprintf(stderr,"%s\t init :", name);
		if ( rc > 0 )
			dfcamac_print_errors(stderr,rc);
		else
			perror(" ");
	}
//	else
//		fputs(" Success...\n",stderr);

	return rc;
}

dev_test_state out_test_err(const char* name, dev_test_state dt)
{
    if( dt != DEV_TEST_SUCCESS )
	{
		fprintf(stderr,"%s\t test : ", name);
		df_print_devtest(stderr,dt);
	}

	return dt;
}

int out_verify_err(const char* name, int rc)
{
	if(rc)
	{
		fprintf(stderr,"%s\t verify :", name);
		if (rc > 0)
			dfcamac_print_errors(stderr,rc);
		else
			perror(" ");
	}
//	else
//		fputs(" Success...\n",stderr);

	return rc;
}

