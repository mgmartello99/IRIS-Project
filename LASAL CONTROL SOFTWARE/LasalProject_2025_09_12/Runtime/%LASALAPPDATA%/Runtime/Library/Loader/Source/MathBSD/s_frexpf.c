// <NewSigmatekCFileOptimize/>
/* s_frexpf.c -- float version of s_frexp.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/s_frexpf.c


#include "MathBSD.h"
#include "math_private.h"


static const float
two25_F = 3.355443200e+07f;	/* 0x4c000000 */


EXTERN_C LslFloatC_t bsd_frexpf(float x, int *eptr)
{
	PROLOGUE_FLOAT();
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = 0x7fffffff & hx;
	*eptr = 0;
	if (ix >= 0x7f800000 || (ix == 0))
	{
		RETURN_FLOAT (x);	/* 0,inf,nan */
	}
	if (ix < 0x00800000)
	{		/* subnormal */
		x *= two25_F;
		GET_FLOAT_WORD(hx, x);
		ix = hx & 0x7fffffff;
		*eptr = -25;
	}
	*eptr += (ix >> 23) - 126;
	hx = (hx & 0x807fffff) | 0x3f000000;
	SET_FLOAT_WORD(x, hx);
	RETURN_FLOAT (x);
	EPILOGUE_FLOAT();
}
