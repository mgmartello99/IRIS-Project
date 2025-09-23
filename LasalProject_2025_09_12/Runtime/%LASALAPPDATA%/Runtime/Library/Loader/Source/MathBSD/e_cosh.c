// <NewSigmatekCFileOptimize/>

/* @(#)e_cosh.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

 // Refactored from FreeBSD source code from
 // https://svn.freebsd.org/base/head/lib/msun/src/e_cosh.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#define bsd_fabs(x)	__builtin_fabs(x)
#endif


static const double one = 1.0, half = 0.5, huge = 1.0e300;


/* __ieee754_cosh(x)
 * Method : 
 * mathematically cosh(x) if defined to be (exp(x)+exp(-x))/2
 *	1. Replace x by |x| (cosh(x) = cosh(-x)). 
 *	2. 
 *		                                        [ exp(x) - 1 ]^2 
 *	    0        <= x <= ln2/2  :  cosh(x) := 1 + -------------------
 *			       			           2*exp(x)
 *
 *		                                  exp(x) +  1/exp(x)
 *	    ln2/2    <= x <= 22     :  cosh(x) := -------------------
 *			       			          2
 *	    22       <= x <= lnovft :  cosh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  cosh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  cosh(x) := huge*huge (overflow)
 *
 * Special cases:
 *	cosh(x) is |x| if x is +INF, -INF, or NaN.
 *	only cosh(0)=1 is exact for finite x.
 */
EXTERN_C double bsd_cosh(double x)
{
	PROLOGUE_DOUBLE();
	double t,w;
	int32_t ix;

	/* High word of |x|. */
	GET_HIGH_WORD(ix,x);
	ix &= 0x7fffffff;

	/* x is INF or NaN */
	if(ix>=0x7ff00000)
	{
		RETURN_DOUBLE (x * x);
	}

	/* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
	if(ix<0x3fd62e43) {
		t = bsd_expm1(bsd_fabs(x));
		w = one+t;
		if (ix<0x3c800000)
		{
			RETURN_DOUBLE (w);	/* cosh(tiny) = 1 */
		}
		RETURN_DOUBLE (one+(t*t)/(w+w));
	}

	/* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
	if (ix < 0x40360000)
	{
		t = bsd_exp(bsd_fabs(x));
		RETURN_DOUBLE (half*t+half/t);
	}

	/* |x| in [22, log(maxdouble)] return half*exp(|x|) */
	if (ix < 0x40862E42)
	{
		RETURN_DOUBLE (half * bsd_exp(bsd_fabs(x)));
	}

	/* |x| in [log(maxdouble), overflowthresold] */
	if (ix<=0x408633CE)
	{
		RETURN_DOUBLE (__ldexp_exp(bsd_fabs(x), -1));
	}

	/* |x| > overflowthresold, cosh(x) overflow */
	RETURN_DOUBLE (huge*huge);
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double cosh(double x) ALIAS(bsd_cosh);
