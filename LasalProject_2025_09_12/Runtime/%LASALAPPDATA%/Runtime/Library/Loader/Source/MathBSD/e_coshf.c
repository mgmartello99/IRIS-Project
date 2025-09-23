// <NewSigmatekCFileOptimize/>
/* e_coshf.c -- float version of e_cosh.c.
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
 // https://svn.freebsd.org/base/head/lib/msun/src/e_coshf.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static const float one = 1.0f, half=0.5f, huge = 1.0e30f;

EXTERN_C LslFloatC_t bsd_coshf(float x)
{
	PROLOGUE_FLOAT();
	float t, w;
	int32_t ix;

	GET_FLOAT_WORD(ix, x);
	ix &= 0x7fffffff;

	/* x is INF or NaN */
	if (ix >= 0x7f800000)
	{
		RETURN_FLOAT(x*x);
	}

	/* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
	if (ix < 0x3eb17218)
	{
		t = lsl_expm1f(lsl_fabsf(x));
		w = one + t;
		if (ix < 0x39800000)
		{
			RETURN_FLOAT(one);	/* cosh(tiny) = 1 */
		}
		RETURN_FLOAT(one + (t*t) / (w + w));
	}

	/* |x| in [0.5*ln2,9], return (exp(|x|)+1/exp(|x|))/2; */
	if (ix < 0x41100000)
	{
		t = lsl_expf(lsl_fabsf(x));
		RETURN_FLOAT (half*t + half / t);
	}

	/* |x| in [9, log(maxfloat)] return half*exp(|x|) */
	if (ix < 0x42b17217)
	{
		RETURN_FLOAT (half * lsl_expf(lsl_fabsf(x)));
	}

	/* |x| in [log(maxfloat), overflowthresold] */
	if (ix <= 0x42b2d4fc)
	{
		RETURN_FLOAT (__ldexp_expf(lsl_fabsf(x), -1));
	}

	/* |x| > overflowthresold, cosh(x) overflow */
	RETURN_FLOAT (huge * huge);
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float coshf(float x) { return L2F_(bsd_coshf(x)); }
