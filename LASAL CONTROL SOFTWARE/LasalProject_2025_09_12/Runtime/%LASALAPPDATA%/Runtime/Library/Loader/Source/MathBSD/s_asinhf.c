// <NewSigmatekCFileOptimize/>
/* s_asinhf.c -- float version of s_asinh.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_asinhf.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static const float
one =  1.0000000000e+00f, /* 0x3F800000 */
ln2 =  6.9314718246e-01f, /* 0x3f317218 */
huge=  1.0000000000e+30f;


EXTERN_C LslFloatC_t bsd_asinhf(float x)
{
	PROLOGUE_FLOAT();
	float t, w;
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x7f800000)
	{
		RETURN_FLOAT (x + x);	/* x is inf or NaN */
	}
	if (ix < 0x31800000)
	{	/* |x|<2**-28 */
		if (huge + x > one)
		{
			RETURN_FLOAT(x);	/* return x inexact except 0 */
		}
	}
	if (ix > 0x4d800000)
	{	/* |x| > 2**28 */
		w = lsl_logf(lsl_fabsf(x)) + ln2;
	}
	else if (ix > 0x40000000)
	{	/* 2**28 > |x| > 2.0 */
		t = lsl_fabsf(x);
		w = lsl_logf((float)2.0*t + one / (lsl_sqrtf(x*x + one) + t));
	}
	else
	{		/* 2.0 > |x| > 2**-28 */
		t = x * x;
		w = lsl_log1pf(lsl_fabsf(x) + t / (one + lsl_sqrtf(one + t)));
	}
	if (hx > 0)
	{
		RETURN_FLOAT(w);
	}
	else
	{
		RETURN_FLOAT(-w);
	}
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float asinhf(float x) { return L2F_(bsd_asinhf(x)); }
