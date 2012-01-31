/*
	<camac/defs.h>
	$Id: defs.h,v 1.1 2010-06-22 05:22:02 gulevich Exp $

	General CAMAC types and constants definitions

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL

	Approved by Vyacheslav V. Mukhortov <vm@inteks.ru> at 1999.02.02
*/

#ifndef  _CAMAC_DEFS_H
# define _CAMAC_DEFS_H

# include <camac/base_defs.h>
#include <stdint.h>
//# include <camac/event_bitmask.h>


typedef u32_t size_t;
typedef unsigned long ulong;
typedef size_t df_timeout_t;

# define DF_TIMEOUT_0	(0)
# define DF_TIMEOUT_INF	((size_t)-1)	/* poll() */
# define DF_TIMEOUT_MAX	(INT_MAX)	/* poll() */

# define DF_TIMEOUT_PTR_0   ((df_timeout_t*) 1ul)
# define DF_TIMEOUT_PTR_INF ((df_timeout_t*)0)


enum	/* camac_condition_bit_shifts */
{
    /* additional logical answer */
    CAMAC_CC_BOOL_SHIFT               =  0,

    /* software errors */
    CAMAC_CC_SYSCALL_ERROR_SHIFT      =  1,
    CAMAC_CC_INVALID_ARG_SHIFT        =  2,
    CAMAC_CC_NOTBINDED_SHIFT          =  3,
    CAMAC_CC_NOTINITED_SHIFT          =  4,

    /* hardware errors */
    CAMAC_CC_IFACE_TIMEOUT_SHIFT      =  5, /* no reply             */
    CAMAC_CC_DATA_XFER_ERROR_SHIFT    =  6, /* data transfer error  */
    CAMAC_CC_VERIFICATION_ERROR_SHIFT =  7, /* too long to describe */
    CAMAC_CC_INTERMEDIATE_ERROR_SHIFT =  8, /* intermediate device error */
    CAMAC_CC_TEST_FAILED_SHIFT        =  9, /* last test was failed */

    /* Station answer.                                        */
    /* These bits can be returned by station operations only. */
    CAMAC_CC_NOT_X_SHIFT              = 10,
    CAMAC_CC_NOT_Q_SHIFT              = 11,

    CAMAC_CC_USER_ERROR0_SHIFT        = 12,
    CAMAC_CC_USER_ERROR1_SHIFT        = 13,
    CAMAC_CC_USER_ERROR2_SHIFT        = 14,
    CAMAC_CC_USER_ERROR3_SHIFT        = 15,
};

enum { CAMAC_CC_BITS = CAMAC_CC_USER_ERROR3_SHIFT+1 };


/* CAMAC condition code bits.                               */
/* These are integral constant expressions whose values are */
/* distinct powers of 2.                                    */
/* Any bit except CAMAC_CC_BOOL is error flag.              */
/* All CAMAC operations must report only those error flags, */
/* which are errors for them.                               */
/*                                                          */
enum camac_cc_bits
{
    CAMAC_CC_OK = 0,
    /* additional logical answer */
    CAMAC_CC_BOOL               = 1<<CAMAC_CC_BOOL_SHIFT,

    /* software errors */
    CAMAC_CC_SYSCALL_ERROR      = 1<<CAMAC_CC_SYSCALL_ERROR_SHIFT,
    CAMAC_CC_INVALID_ARG        = 1<<CAMAC_CC_INVALID_ARG_SHIFT  ,
    CAMAC_CC_NOTBINDED          = 1<<CAMAC_CC_NOTBINDED_SHIFT    ,
    CAMAC_CC_NOTINITED          = 1<<CAMAC_CC_NOTINITED_SHIFT    ,

    __CAMAC_CC_SOFT_ERRORS      =
		  CAMAC_CC_SYSCALL_ERROR
		| CAMAC_CC_INVALID_ARG
		| CAMAC_CC_NOTBINDED
		| CAMAC_CC_NOTINITED,


    /* hardware errors */
    CAMAC_CC_IFACE_TIMEOUT      = 1<<CAMAC_CC_IFACE_TIMEOUT_SHIFT     ,
    CAMAC_CC_DATA_XFER_ERROR    = 1<<CAMAC_CC_DATA_XFER_ERROR_SHIFT   ,
    CAMAC_CC_VERIFICATION_ERROR = 1<<CAMAC_CC_VERIFICATION_ERROR_SHIFT,
    CAMAC_CC_INTERMEDIATE_ERROR = 1<<CAMAC_CC_INTERMEDIATE_ERROR_SHIFT,
    CAMAC_CC_TEST_FAILED        = 1<<CAMAC_CC_TEST_FAILED_SHIFT,

    __CAMAC_CC_HARD_ERRORS      =
		  CAMAC_CC_IFACE_TIMEOUT
		| CAMAC_CC_DATA_XFER_ERROR
		| CAMAC_CC_VERIFICATION_ERROR
		| CAMAC_CC_INTERMEDIATE_ERROR
		| CAMAC_CC_TEST_FAILED,


    CAMAC_CC_ERRORS_ONLY        =
		  __CAMAC_CC_SOFT_ERRORS
		| __CAMAC_CC_HARD_ERRORS,


    /* Station answer.                                        */
    /* These bits may be returned by station operations only. */
    CAMAC_CC_NOT_X              = 1<<CAMAC_CC_NOT_X_SHIFT,
    CAMAC_CC_NOT_Q              = 1<<CAMAC_CC_NOT_Q_SHIFT,
    CAMAC_CC_NOT_XQ             = CAMAC_CC_NOT_X | CAMAC_CC_NOT_Q,

    /* User errors extension */
    CAMAC_CC_USER_ERROR0        = 1<<CAMAC_CC_USER_ERROR0_SHIFT,
    CAMAC_CC_USER_ERROR1        = 1<<CAMAC_CC_USER_ERROR1_SHIFT,
    CAMAC_CC_USER_ERROR2        = 1<<CAMAC_CC_USER_ERROR2_SHIFT,
    CAMAC_CC_USER_ERROR3        = 1<<CAMAC_CC_USER_ERROR3_SHIFT,

    CAMAC_CC_USER_ERRORS        =
		  CAMAC_CC_USER_ERROR0 | CAMAC_CC_USER_ERROR1
		| CAMAC_CC_USER_ERROR2 | CAMAC_CC_USER_ERROR3,

    /* error for user - errors and station answer */
    CAMAC_CC_ERRORS             =
		CAMAC_CC_ERRORS_ONLY | CAMAC_CC_NOT_XQ | CAMAC_CC_USER_ERRORS,

    CAMAC_CC_ALL                =
		CAMAC_CC_ERRORS | CAMAC_CC_BOOL,

    /* this enum must has size of int */
    __CAMAC_CC_INT              = -1
};


enum { __DEV_TEST_IM_MASK            = 0x80 };


typedef enum dev_test_state
{

	DEV_TEST_SUCCESS                 = 0,

	DEV_TEST_NOTBINDED               = 1,
	DEV_TEST_ABSENT                  = 2,
	DEV_TEST_BROKEN                  = 3,
	DEV_TEST_INCOMPLETE              = 4, /* for complex or virtual devices */

	/* inaccessible due to the intermediate device failure */
	DEV_TEST_INTERMEDIATE_NOTBINDED  = DEV_TEST_NOTBINDED |__DEV_TEST_IM_MASK,
	DEV_TEST_INTERMEDIATE_ABSENT     = DEV_TEST_ABSENT    |__DEV_TEST_IM_MASK,
	DEV_TEST_INTERMEDIATE_BROKEN     = DEV_TEST_BROKEN    |__DEV_TEST_IM_MASK,
	DEV_TEST_INTERMEDIATE_INCOMPLETE = DEV_TEST_INCOMPLETE|__DEV_TEST_IM_MASK,

	DEV_TEST_SYSCALL_ERROR           = -1,

	/* aliases */
	DEV_TEST_OK   = DEV_TEST_SUCCESS,
	DEV_TEST_NULL = DEV_TEST_NOTBINDED,

} dev_test_state;


//Converts dev_test_state to intermediate values for upper level.
inline dev_test_state dev_test_to_intermediate(dev_test_state dt)
{
	return dt > 0 ? (dev_test_state)(dt|__DEV_TEST_IM_MASK) : dt;
}

struct camac_geometry
{
    camac_i_t i;            /* amount of interface cards within driver */
    camac_c_t c;            /* amount of channels within interface     */
};
/*
 "phys_iface_number" = "through_crate_number" / c;
 "phys_crate_number" = "through_crate_number" % c;
*/


struct camac_address
{
    char*		driver_name;
    camac_i_t	iface;
    camac_c_t	crate;
    camac_n_t	station;
};



typedef u32_t camac_af_t;

enum
{
    CAMAC_AF_A_SHIFT = 0,
    CAMAC_AF_F_SHIFT = CAMAC_AF_A_SHIFT+CAMAC_A_BITS,
    CAMAC_AF_W_SHIFT = CAMAC_AF_F_SHIFT+CAMAC_F_BITS,
};
enum
{
    CAMAC_AF_A_MASK  = CAMAC_A_MASK<<CAMAC_AF_A_SHIFT,
    CAMAC_AF_F_MASK  = CAMAC_F_MASK<<CAMAC_AF_F_SHIFT,
    CAMAC_AF_W_MASK  =            1<<CAMAC_AF_W_SHIFT,

    CAMAC_AF_MASK    = CAMAC_AF_A_MASK | CAMAC_AF_F_MASK | CAMAC_AF_W_MASK,


    __CAMAC_AF_UINT  = -1U	/* this masks must have size of uint */
};

# define CAMAC_MAKE_AF(a,f)	\
	( ( ((a) << CAMAC_AF_A_SHIFT) | ((f) << CAMAC_AF_F_SHIFT) )	\
		& ( CAMAC_AF_A_MASK | CAMAC_AF_F_MASK ) )

# define CAMAC_MAKE_AF16(a,f)	 CAMAC_MAKE_AF((a),(f))
# define CAMAC_MAKE_AF24(a,f)	(CAMAC_MAKE_AF((a),(f)) | CAMAC_AF_W_MASK)


# define CAMAC_INIT_AF(a,f)	CAMAC_MAKE_AF(a,f)
# define CAMAC_INIT_AF16(a,f)	CAMAC_MAKE_AF16(a,f)
# define CAMAC_INIT_AF24(a,f)	CAMAC_MAKE_AF24(a,f)


/* af field extractors */
extern __inline__ camac_a_t camac_af_a(camac_af_t _af)
{
    return (_af>>CAMAC_AF_A_SHIFT) & CAMAC_A_MASK;
}

extern __inline__ camac_f_t camac_af_f(camac_af_t _af)
{
    return (_af>>CAMAC_AF_F_SHIFT) & CAMAC_F_MASK;
}

extern __inline__ int is_camac_af_nodata(camac_af_t af)
{
    return af & (CAMAC_F8<<CAMAC_AF_F_SHIFT);
}

extern __inline__ int is_camac_af_data(camac_af_t af)
{
    return ! is_camac_af_nodata(af);
}

extern __inline__ int is_camac_af_read(camac_af_t af)
{
    return ! ( af & ( (CAMAC_F16|CAMAC_F8) << CAMAC_AF_F_SHIFT ) );
}

extern __inline__ int is_camac_af_write(camac_af_t af)
{
    return is_camac_af_data(af) &&
	af & (CAMAC_F16<<CAMAC_AF_F_SHIFT);
}



typedef u16_t camac_cna_t;

#if 0
typedef struct
{
    u16_t a:CAMAC_A_BITS;
    u16_t n:CAMAC_N_BITS;
    u16_t c:CAMAC_C_BITS;
    u16_t  : 16-(CAMAC_A_BITS+CAMAC_N_BITS+CAMAC_C_BITS);
} camac_cna_s;
#endif

enum
{
    CAMAC_CNA_A_SHIFT = 0,
    CAMAC_CNA_N_SHIFT = CAMAC_CNA_A_SHIFT+CAMAC_A_BITS,
    CAMAC_CNA_C_SHIFT = CAMAC_CNA_N_SHIFT+CAMAC_N_BITS
};


# define __CAMAC_MAKE_CNA(c,n,a) \
	( ((a) << CAMAC_CNA_A_SHIFT) | ( (n) << CAMAC_CNA_N_SHIFT ) \
	  | ( (c) << CAMAC_CNA_C_SHIFT ) )

# define CAMAC_MAKE_CNA(a)	( ((a)&CAMAC_A_MASK) << CAMAC_CNA_A_SHIFT )

# define CAMAC_INIT_CNA(a)	CAMAC_MAKE_CNA(a)

/* cna field extractors */
extern __inline__ camac_c_t camac_cna_c(camac_cna_t _cna)
{
    return (_cna>>CAMAC_CNA_C_SHIFT) & CAMAC_C_MASK;
}

extern __inline__ camac_n_t camac_cna_n(camac_cna_t _cna)
{
    return (_cna>>CAMAC_CNA_N_SHIFT) & CAMAC_N_MASK;
}

extern __inline__ camac_a_t camac_cna_a(camac_cna_t _cna)
{
    return (_cna>>CAMAC_CNA_A_SHIFT) & CAMAC_A_MASK;
}



#if 0	/* cnaf */
typedef u32_t camac_cnaf_t;

#if 0
typedef struct
{
    camac_cna_u cna;
    u16_t       f : CAMAC_F_BITS;
} camac_cnaf_s;
#endif

enum
{
    CAMAC_CNAF_CNA_SHIFT = 0,

    CAMAC_CNAF_A_SHIFT = CAMAC_CNAF_CNA_SHIFT+CAMAC_CNA_A_SHIFT,
    CAMAC_CNAF_N_SHIFT = CAMAC_CNAF_CNA_SHIFT+CAMAC_CNA_N_SHIFT,
    CAMAC_CNAF_C_SHIFT = CAMAC_CNAF_CNA_SHIFT+CAMAC_CNA_C_SHIFT,
    CAMAC_CNAF_F_SHIFT = 16
};


# define CAMAC_MAKE_CNAF(a,f) \
	( CAMAC_MAKE_CNA(a) | ( ((f) & CAMAC_F_MASK) << CAMAC_CNAF_F_SHIFT ) )

# define CAMAC_INIT_CNAF(a,f)	CAMAC_MAKE_CNAF((a),(f))


/* cnaf field extractors */
extern __inline__ camac_cna_t camac_cnaf_cna(camac_cnaf_t cnaf)
{
    return cnaf;
}

extern __inline__ camac_c_t camac_cnaf_c(camac_cnaf_t cnaf)
{
    return (cnaf>>CAMAC_CNAF_C_SHIFT) & CAMAC_C_MASK;
}

extern __inline__ camac_n_t camac_cnaf_n(camac_cnaf_t cnaf)
{
    return (cnaf>>CAMAC_CNAF_N_SHIFT) & CAMAC_N_MASK;
}

extern __inline__ camac_a_t camac_cnaf_a(camac_cnaf_t cnaf)
{
    return (cnaf>>CAMAC_CNAF_A_SHIFT) & CAMAC_A_MASK;
}

extern __inline__ camac_f_t camac_cnaf_f(camac_cnaf_t cnaf)
{
    return (cnaf>>CAMAC_CNAF_F_SHIFT) & CAMAC_F_MASK;
}


extern __inline__ int is_camac_cnaf_data(camac_cnaf_t cnaf)
{
    return ! (cnaf & (CAMAC_F8<<CAMAC_CNAF_F_SHIFT));
}

extern __inline__ int is_camac_cnaf_read(camac_cnaf_t cnaf)
{
    return ! ( cnaf & ( (CAMAC_F16|CAMAC_F8) << CAMAC_CNAF_F_SHIFT ) );
}

extern __inline__ int is_camac_cnaf_write(camac_cnaf_t cnaf)
{
    return is_camac_cnaf_data(cnaf) &&
	cnaf & (CAMAC_F16<<CAMAC_CNAF_F_SHIFT);
}
#endif	/* cnaf */


enum camac_stop_condition
{
    CAMAC_STOP_NONE      = 0,   /* unconditional block transfer    */

    CAMAC_STOP_ON_ERRORS =      /* include notX                    */
		CAMAC_CC_ERRORS_ONLY | CAMAC_CC_NOT_X,          /* trick   */

    CAMAC_STOP_ON_NOT_Q  =      /* errors and notQ                 */
	CAMAC_CC_ERRORS_ONLY | CAMAC_CC_NOT_XQ,         /* trick   */
};



struct camac_block16
{
    u16_t* data;
    size_t count;
    enum camac_stop_condition stop_cond;
    int cc_bits;		/* should be zeroed on start */
};

struct camac_block24
{
    u32_t* data;
    size_t count;
    enum camac_stop_condition stop_cond;
    int cc_bits;		/* should be zeroed on start */
};


struct camac_af_list	/* af list item */
{
    camac_af_t af;		/* CAMAC_{MAKE,INIT}_AF{16,24} */
    u32_t data;
};


enum camac_lam_check_methods
{
    CAMAC_LAM_CHECK_NONE         ,  /* no LAM check method          */
    CAMAC_LAM_CHECK_BY_Q         ,  /* Q on LAM                     */
    CAMAC_LAM_CHECK_BY_NOTQ      ,  /* NotQ on LAM                  */
    CAMAC_LAM_CHECK_BY_BITMASK   ,  /* (data & bitmask) != 0 on LAM */
    CAMAC_LAM_CHECK_BY_NOTBITMASK,  /* (data & bitmask) == 0 on LAM */

    CAMAC_LAM_CHECK_BY_Q_AND_NOTX_IS_NOT_ERROR,

    CAMAC_LAM_CHECK_METHODS,        /* amount of methods */

    /* this enum must has size at least of short int */
    __CAMAC_LAM_CHECK_SHORT = 1<<15
};

struct camac_lam_check_method
{
    camac_af_t af;
    u32_t bitmask;
    enum camac_lam_check_methods method;
};



/* converts boolean integer to CAMAC_CC_BOOL bit */
inline int camac_cc_bool(unsigned long arg)
{
    return arg ? CAMAC_CC_BOOL : 0;
}


/* safe version of operator '|' */
extern __inline__ int camac_combine_errors(int rc1, int rc2)
{
    return rc1<0 ? rc1 : rc2<0 ? rc2 : rc1|rc2;
}

extern __inline__ int camac_combine_errors1(int rc1, int cc_bits2)
{
    return rc1<0 ? rc1 : rc1|cc_bits2;
}


enum
{
    CAMAC_LOCK_STATION              = 1<<0,
    CAMAC_LOCK_C                    = 1<<1,
    CAMAC_LOCK_Z                    = 1<<2,
    /* reserved                     = 1<<3, */
    /* reserved                     = 1<<4, */


    CAMAC_OPEN_STATION_NONBLOCK     = 1<<5,
    CAMAC_OPEN_STATION_NOLOCK       = 1<<6,

    CAMAC_OPEN_STATION_C_SENSITIVE  = 1<<7,
    CAMAC_OPEN_STATION_Z_SENSITIVE  = 1<<8,

    CAMAC_OPEN_STATION_LOCK_C       = CAMAC_LOCK_C,
    CAMAC_OPEN_STATION_LOCK_Z       = CAMAC_LOCK_Z,



    CAMAC_LOCK_CZ  = CAMAC_LOCK_C       | CAMAC_LOCK_Z,
    CAMAC_LOCK_ALL = CAMAC_LOCK_STATION | CAMAC_LOCK_CZ,


    CAMAC_OPEN_STATION_CZ_SENSITIVE =
		  CAMAC_OPEN_STATION_C_SENSITIVE
		| CAMAC_OPEN_STATION_Z_SENSITIVE,

    CAMAC_OPEN_STATION_LOCK_CZ      =
		  CAMAC_OPEN_STATION_LOCK_C
		| CAMAC_OPEN_STATION_LOCK_Z,

    CAMAC_OPEN_STATION_FLAGS        =
		  CAMAC_OPEN_STATION_NONBLOCK
		| CAMAC_OPEN_STATION_NOLOCK
		| CAMAC_OPEN_STATION_CZ_SENSITIVE
		| CAMAC_OPEN_STATION_LOCK_CZ
};



enum camac_readwrite { CAMAC_READ=0, CAMAC_WRITE=CAMAC_F16 };


#endif	/* _CAMAC_DEFS_H */

