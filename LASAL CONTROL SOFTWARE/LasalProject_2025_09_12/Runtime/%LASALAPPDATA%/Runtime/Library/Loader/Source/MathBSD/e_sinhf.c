// <NewSigmatekCFileOptimize/>
/* e_sinhf.c -- float version of e_sinh.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_sinhf.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static const float one = 1.0f, shuge = 1.0e37f;

EXTERN_C LslFloatC_t bsd_sinhf(float x)
{
	PROLOGUE_FLOAT();
	float t, h;
	int32_t ix, jx;

	GET_FLOAT_WORD(jx, x);
	ix = jx & 0x7fffffff;

	/* x is INF or NaN */
	if (ix >= 0x7f800000)
	{
		RETURN_FLOAT(x + x);
	}

	h = 0.5;
	if (jx < 0) h = -h;
	/* |x| in [0,9], return sign(x)*0.5*(E+E/(E+1))) */
	if (ix < 0x41100000)
	{		/* |x|<9 */
		if (ix < 0x39800000) 		/* |x|<2**-12 */
			if (shuge + x > one)
			{
				RETURN_FLOAT(x);/* sinh(tiny) = tiny with inexact */
			}
		t = lsl_expm1f(lsl_fabsf(x));
		if (ix < 0x3f800000)
		{
			RETURN_FLOAT(h * ((float)2.0*t - t * t / (t + one)));
		}
		RETURN_FLOAT(h * (t + t / (t + one)));
	}

	/* |x| in [9, logf(maxfloat)] return 0.5*exp(|x|) */
	if (ix < 0x42b17217)
	{
		RETURN_FLOAT(h * lsl_expf(lsl_fabsf(x)));
	}

	/* |x| in [logf(maxfloat), overflowthresold] */
	if (ix <= 0x42b2d4fc)
	{
		RETURN_FLOAT(h * 2.0F*__ldexp_expf(lsl_fabsf(x), -1));
	}

	/* |x| > overflowthresold, sinh(x) overflow */
	RETURN_FLOAT(x * shuge);
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float sinhf(float x) { return L2F_(bsd_sinhf(x)); }
