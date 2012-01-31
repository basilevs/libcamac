/*
	<camac/base_defs.h>
	$Id: base_defs.h,v 1.2 2010-06-22 08:38:56 gulevich Exp $

	General CAMAC types and constants.
	Base definitions.

	(C) 1999 Dmitry A. Fedorov <dm.fedorov@gmail.com>
	Copying policy: GNU LGPL
*/

#ifndef  _CAMAC_BASE_DEFS_H
# define _CAMAC_BASE_DEFS_H

#include <stdint.h>

typedef uint8_t u8_t;
typedef uint16_t u16_t;
typedef uint32_t u32_t;


typedef u16_t ushort;
/* + types */
typedef u16_t camac_crate_t; /* global crate number (within driver)     */

/* physical address types */
typedef u8_t  camac_i_t;      /* interface (card) number within driver   */
typedef u8_t  camac_c_t;      /* channel (crate) number within interface */
typedef u8_t  camac_n_t;      /* slot (station) number                   */
typedef u8_t  camac_a_t;      /* subaddress                              */
typedef u8_t  camac_f_t;      /* function                                */

typedef u8_t  camac_g_t;      /* group number (internal use only)        */

typedef u32_t camac_c_mask_t; /* channels (crates) mask within interface */
typedef u32_t camac_n_mask_t; /* station mask within crate               */
typedef u8_t  camac_g_mask_t; /* group mask                              */
/* - types */


enum	/* limits */
{
    CAMAC_N_MIN =  1,
    CAMAC_N_MAX = 23,

    CAMAC_A_MIN =  0,
    CAMAC_A_MAX = 15,

    CAMAC_F_MIN =  0,
    CAMAC_F_MAX = 31,

    CAMAC_CRATE_SLOTS = 24,	/* includes slot 0      */

    CAMAC_IFACE_CHANNELS = 32	/* u32_t camac_c_mask_t */
};


enum	/* bits */
{
    CAMAC_C_BITS = 5,	/* u32_t */
    CAMAC_N_BITS = 5,
    CAMAC_A_BITS = 4,
    CAMAC_F_BITS = 5,

    CAMAC_C_MASK = 0x1F,
    CAMAC_N_MASK = 0x1F,
    CAMAC_A_MASK = 0x0F,
    CAMAC_F_MASK = 0x1F
};


enum	/* some function bits */
{
    CAMAC_F16 = 0x10,
    CAMAC_F8  = 0x08
};



static __inline__ int is_camac_f_nodata(camac_f_t f)
{
    return f & CAMAC_F8;
}

static __inline__ int is_camac_f_data(camac_f_t f)
{
    return !is_camac_f_nodata(f);
}

static __inline__ int is_camac_f_read(camac_f_t f)
{
    return ! ( f & (CAMAC_F16|CAMAC_F8) );
}

static __inline__ int is_camac_f_write(camac_f_t f)
{
    return f&CAMAC_F16 && is_camac_f_data(f);
}



/* returns error flag */
static __inline__ int camac_check_channel(camac_c_t c)
{
    return c>CAMAC_IFACE_CHANNELS;
}

static __inline__ int camac_check_slot(camac_n_t n)
{
    return n<CAMAC_N_MIN || n>CAMAC_N_MAX;
}

static __inline__ int camac_check_subaddress(camac_a_t a)
{
    return a>CAMAC_A_MAX;
}

static __inline__ int camac_check_function(camac_f_t f)
{
    return f>CAMAC_F_MAX;
}


#endif	/* _CAMAC_BASE_DEFS_H */

