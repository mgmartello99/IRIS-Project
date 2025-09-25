// <NewSigmatekCFileOptimize/>
/* e_acosf.c -- float version of e_acos.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_acosf.c


#include "MathBSD.h"
#include "math_private.h"


static const float
one =  1.0000000000e+00f, /* 0x3F800000 */
pi =  3.1415925026e+00f, /* 0x40490fda */
pio2_hi =  1.5707962513e+00f; /* 0x3fc90fda */
static volatile float
pio2_lo =  7.5497894159e-08f; /* 0x33a22168 */
static const float
pS0 =  1.6666586697e-01f,
pS1 = -4.2743422091e-02f,
pS2 = -8.6563630030e-03f,
qS1 = -7.0662963390e-01f;


EXTERN_C LslFloatC_t bsd_acosf(float x)
{
	PROLOGUE_FLOAT();
	float z, p, q, r, w, s, c, df;
	int32_t hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x3f800000)
	{		/* |x| >= 1 */
		if (ix == 0x3f800000)
		{	/* |x| == 1 */
			if (hx > 0)
			{
				RETURN_FLOAT (0.0);	/* acos(1) = 0 */
			}
			else
			{
				RETURN_FLOAT (pi + (float)2.0*pio2_lo);	/* acos(-1)= pi */
			}
		}
		RETURN_FLOAT ((x - x) / (x - x));		/* acos(|x|>1) is NaN */
	}
	if (ix < 0x3f000000)
	{	/* |x| < 0.5 */
		if (ix <= 0x32800000)
		{
			RETURN_FLOAT (pio2_hi + pio2_lo);/*if|x|<2**-26*/
		}
		z = x * x;
		p = z * (pS0 + z * (pS1 + z * pS2));
		q = one + z * qS1;
		r = p / q;
		RETURN_FLOAT (pio2_hi - (x - (pio2_lo - x * r)));
	}
	else  if (hx < 0)
	{		/* x < -0.5 */
		z = (one + x)*(float)0.5;
		p = z * (pS0 + z * (pS1 + z * pS2));
		q = one + z * qS1;
		s = lsl_sqrtf(z);
		r = p / q;
		w = r * s - pio2_lo;
		RETURN_FLOAT(pi - (float)2.0*(s + w));
	}
	else
	{			/* x > 0.5 */
		int32_t idf;
		z = (one - x)*(float)0.5;
		s = lsl_sqrtf(z);
		df = s;
		GET_FLOAT_WORD(idf, df);
		SET_FLOAT_WORD(df, idf & 0xfffff000);
		c = (z - df * df) / (s + df);
		p = z * (pS0 + z * (pS1 + z * pS2));
		q = one + z * qS1;
		r = p / q;
		w = r * s + c;
		RETURN_FLOAT ((float)2.0*(df + w));
	}
	EPILOGUE_FLOAT();
}
