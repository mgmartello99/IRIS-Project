// <NewSigmatekCFileOptimize/>
/* @(#)s_asinh.c 5.1 93/09/24 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_asinh.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#define bsd_fabs(x)	__builtin_fabs(x)
#endif


static const double
one = 1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
ln2 = 6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
huge = 1.00000000000000000000e+300;


/* asinh(x)
 * Method :
 *	Based on
 *		asinh(x) = sign(x) * log [ |x| + sqrt(x*x+1) ]
 *	we have
 *	asinh(x) := x  if  1+x*x=1,
 *		 := sign(x)*(log(x)+ln2)) for large |x|, else
 *		 := sign(x)*log(2|x|+1/(|x|+sqrt(x*x+1))) if|x|>2, else
 *		 := sign(x)*log1p(|x| + x^2/(1 + sqrt(1+x^2)))
 */
EXTERN_C double bsd_asinh(double x)
{
	PROLOGUE_DOUBLE();
	double t, w;
	int32_t hx, ix;
	GET_HIGH_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix >= 0x7ff00000)
	{
		RETURN_DOUBLE (x + x);	/* x is inf or NaN */
	}
	if (ix < 0x3e300000)
	{	/* |x|<2**-28 */
		if (huge + x > one)
		{
			RETURN_DOUBLE (x);	/* return x inexact except 0 */
		}
	}
	if (ix > 0x41b00000)
	{	/* |x| > 2**28 */
		w = bsd_log(bsd_fabs(x)) + ln2;
	}
	else if (ix > 0x40000000)
	{	/* 2**28 > |x| > 2.0 */
		t = bsd_fabs(x);
		w = bsd_log(2.0*t + one / (bsd_sqrt(x*x + one) + t));
	}
	else
	{		/* 2.0 > |x| > 2**-28 */
		t = x * x;
		w = bsd_log1p(bsd_fabs(x) + t / (one + bsd_sqrt(one + t)));
	}
	if (hx > 0)
	{
		RETURN_DOUBLE (w);
	}
	else
	{
		RETURN_DOUBLE (-w);
	}
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double asinh(double x) ALIAS(bsd_asinh);
