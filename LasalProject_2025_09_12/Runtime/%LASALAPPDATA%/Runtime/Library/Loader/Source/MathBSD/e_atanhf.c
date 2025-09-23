// <NewSigmatekCFileOptimize/>
/* e_atanhf.c -- float version of e_atanh.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_atanhf.c


#include "MathBSD.h"
#include "math_private.h"

static const float one = 1.0, huge = 1e30f;

static const float zero = 0.0f;

EXTERN_C LslFloatC_t bsd_atanhf(float x)
{
	PROLOGUE_FLOAT();
	float t;
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix > 0x3f800000) 		/* |x|>1 */
	{
		RETURN_FLOAT((x - x) / (x - x));
	}
	if (ix == 0x3f800000)
	{
		RETURN_FLOAT(x / zero);
	}
	if (ix<0x31800000 && (huge + x)>zero)
	{
		RETURN_FLOAT(x);	/* x<2**-28 */
	}
	SET_FLOAT_WORD(x, ix);
	if (ix < 0x3f000000)
	{		/* x < 0.5 */
		t = x + x;
		t = (float)0.5*lsl_log1pf(t + t * x / (one - x));
	}
	else
		t = (float)0.5*lsl_log1pf((x + x) / (one - x));
	if (hx >= 0)
	{
		RETURN_FLOAT(t);
	}
	else
	{
		RETURN_FLOAT(-t);
	}
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float atanhf(float x) { return L2F_(bsd_atanhf(x)); }
