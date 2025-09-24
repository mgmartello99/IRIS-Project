// <NewSigmatekCFileOptimize/>

/* @(#)e_sinh.c 1.3 95/01/18 */
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
 // https://svn.freebsd.org/base/head/lib/msun/src/e_sinh.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#define bsd_fabs(x)	__builtin_fabs(x)
#endif


static const double one = 1.0, shuge = 1.0e307;


/* __ieee754_sinh(x)
 * Method : 
 * mathematically sinh(x) if defined to be (exp(x)-exp(-x))/2
 *	1. Replace x by |x| (sinh(-x) = -sinh(x)). 
 *	2. 
 *		                                    E + E/(E+1)
 *	    0        <= x <= 22     :  sinh(x) := --------------, E=expm1(x)
 *			       			        2
 *
 *	    22       <= x <= lnovft :  sinh(x) := exp(x)/2 
 *	    lnovft   <= x <= ln2ovft:  sinh(x) := exp(x/2)/2 * exp(x/2)
 *	    ln2ovft  <  x	    :  sinh(x) := x*shuge (overflow)
 *
 * Special cases:
 *	sinh(x) is |x| if x is +INF, -INF, or NaN.
 *	only sinh(0)=0 is exact for finite x.
 */
EXTERN_C double bsd_sinh(double x)
{
	PROLOGUE_DOUBLE();
	double t,h;
	int32_t ix,jx;

	/* High word of |x|. */
	GET_HIGH_WORD(jx,x);
	ix = jx&0x7fffffff;

	/* x is INF or NaN */
	if(ix>=0x7ff00000)
	{
		RETURN_DOUBLE (x + x);
	}

	h = 0.5;
	if (jx<0) h = -h;
	/* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
	if (ix < 0x40360000) {		/* |x|<22 */
		if (ix<0x3e300000) 		/* |x|<2**-28 */
		if(shuge+x>one)
		{
			RETURN_DOUBLE (x);/* sinh(tiny) = tiny with inexact */
		}
		t = bsd_expm1(bsd_fabs(x));
		if(ix<0x3ff00000)
		{
			RETURN_DOUBLE (h * (2.0*t - t * t / (t + one)));
		}
		RETURN_DOUBLE (h*(t+t/(t+one)));
	}

	/* |x| in [22, log(maxdouble)] return 0.5*exp(|x|) */
	if (ix < 0x40862E42)
	{
		RETURN_DOUBLE (h * bsd_exp(bsd_fabs(x)));
	}

	/* |x| in [log(maxdouble), overflowthresold] */
	if (ix<=0x408633CE)
	{
		RETURN_DOUBLE (h * 2.0*__ldexp_exp(bsd_fabs(x), -1));
	}

	/* |x| > overflowthresold, sinh(x) overflow */
	RETURN_DOUBLE (x*shuge);
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double sinh(double x) ALIAS(bsd_sinh);
