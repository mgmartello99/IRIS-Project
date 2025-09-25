// <NewSigmatekCFileOptimize/>
/* e_acoshf.c -- float version of e_acosh.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_acoshf.c


#include "MathBSD.h"
#include "math_private.h"

static const float
one	= 1.0f,
ln2	= 6.9314718246e-01f;  /* 0x3f317218 */

EXTERN_C LslFloatC_t bsd_acoshf(float x)
{
	PROLOGUE_FLOAT();
	float t;
	int32_t hx;
	GET_FLOAT_WORD(hx, x);
	if (hx < 0x3f800000)
	{		/* x < 1 */
		RETURN_FLOAT((x - x) / (x - x));
	}
	else if (hx >= 0x4d800000)
	{	/* x > 2**28 */
		if (hx >= 0x7f800000)
		{	/* x is inf of NaN */
			RETURN_FLOAT(x + x);
		}
		else
		{
			RETURN_FLOAT(lsl_logf(x) + ln2);	/* acosh(huge)=log(2x) */
		}
	}
	else if (hx == 0x3f800000)
	{
		RETURN_FLOAT(0.0f);			/* acosh(1) = 0 */
	}
	else if (hx > 0x40000000)
	{	/* 2**28 > x > 2 */
		t = x * x;
		RETURN_L2F (bsd_logf(2.0f*x - one / (x + lsl_sqrtf(t - one))));
	}
	else
	{			/* 1<x<2 */
		t = x - one;
		RETURN_L2F (bsd_log1pf(t + lsl_sqrtf(2.0f*t + t * t)));
	}
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float acoshf(float x) { return L2F_(bsd_acoshf(x)); }
