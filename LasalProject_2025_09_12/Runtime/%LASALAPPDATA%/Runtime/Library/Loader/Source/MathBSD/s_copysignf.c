/* s_copysignf.c -- float version of s_copysign.c.
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

// Refactored from FreeBSD source code from
// https://svn.freebsd.org/base/head/lib/msun/src/s_copysignf.c


#include "MathBSD.h"
#include "math_private.h"


/*
 * copysignf(float x, float y)
 * copysignf(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */
EXTERN_C float _bsd_copysignf(float x, float y)
{
	u_int32_t ix, iy;
	GET_FLOAT_WORD(ix, x);
	GET_FLOAT_WORD(iy, y);
	SET_FLOAT_WORD(x, (ix & 0x7fffffff) | (iy & 0x80000000));
	return x;
}
