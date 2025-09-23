// <NewSigmatekCFileOptimize/>
/* e_asinf.c -- float version of e_asin.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_asinf.c


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
huge =  1.000e+30f,
	/* coefficient for R(x^2) */
pS0 =  1.6666586697e-01f,
pS1 = -4.2743422091e-02f,
pS2 = -8.6563630030e-03f,
qS1 = -7.0662963390e-01f;

static const double
pio2 =  1.570796326794896558e+00;


EXTERN_C LslFloatC_t bsd_asinf(float x)
{
	PROLOGUE_FLOAT();
	double s;
	float t, w, p, q;
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x3f800000)
	{		/* |x| >= 1 */
		if (ix == 0x3f800000)		/* |x| == 1 */
		{
			RETURN_FLOAT(x * pio2);		/* asin(+-1) = +-pi/2 with inexact */
		}
		RETURN_FLOAT ((x - x) / (x - x));		/* asin(|x|>1) is NaN */
	}
	else if (ix < 0x3f000000)
	{	/* |x|<0.5 */
		if (ix < 0x39800000)
		{		/* |x| < 2**-12 */
			if (huge + x > one)
			{
				RETURN_FLOAT (x);			/* return x with inexact if x!=0*/
			}
		}
		t = x * x;
		p = t * (pS0 + t * (pS1 + t * pS2));
		q = one + t * qS1;
		w = p / q;
		RETURN_FLOAT (x + x * w);
	}
	/* 1> |x|>= 0.5 */
	w = one - lsl_fabsf(x);
	t = w * (float)0.5;
	p = t * (pS0 + t * (pS1 + t * pS2));
	q = one + t * qS1;
	s = bsd_sqrt(t);
	w = p / q;
	t = (float)(pio2 - 2.0*(s + s * w));
	if (hx > 0)
	{
		RETURN_FLOAT (t);
	}
	else
	{
		RETURN_FLOAT (-t);
	}
	EPILOGUE_FLOAT();
}
