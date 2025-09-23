// <NewSigmatekCFileOptimize/>
/* s_tanhf.c -- float version of s_tanh.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_tanhf.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static const volatile float tiny = 1.0e-30f;
static const float one=1.0, two=2.0, huge = 1.0e30f;

EXTERN_C LslFloatC_t bsd_tanhf(float x)
{
	PROLOGUE_FLOAT();
	float t, z;
	int32_t jx, ix;

	GET_FLOAT_WORD(jx, x);
	ix = jx & 0x7fffffff;

	/* x is INF or NaN */
	if (ix >= 0x7f800000)
	{
		if (jx >= 0)
		{
			RETURN_FLOAT(one / x + one);    /* tanh(+-inf)=+-1 */
		}
		else
		{
			RETURN_FLOAT(one / x - one);    /* tanh(NaN) = NaN */
		}
	}

	/* |x| < 9 */
	if (ix < 0x41100000)
	{		/* |x|<9 */
		if (ix < 0x39800000)
		{	/* |x|<2**-12 */
			if (huge + x > one)
				RETURN_FLOAT(x); /* tanh(tiny) = tiny with inexact */
		}
		if (ix >= 0x3f800000)
		{	/* |x|>=1  */
			t = lsl_expm1f(two*lsl_fabsf(x));
			z = one - two / (t + two);
		}
		else
		{
			t = lsl_expm1f(-two * lsl_fabsf(x));
			z = -t / (t + two);
		}
	/* |x| >= 9, return +-1 */
	}
	else
	{
		z = one - tiny;		/* raise inexact flag */
	}
	RETURN_FLOAT((jx >= 0) ? z : -z);
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float tanhf(float x) { return L2F_(bsd_tanhf(x)); }
