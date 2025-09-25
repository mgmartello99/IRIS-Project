// <NewSigmatekCFileOptimize/>
/* @(#)s_cos.c 5.1 93/09/24 */
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

// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/s_cos.c

#include "MathBSD.h"
#include "math_private.h"
#include "e_rem_pio2.inl.h"

#if OPT_USE_FPU_COS_
#error FPU precision for cosine function is below expectation. Plz use Soft FP.
See: https://software.intel.com/en-us/forums/intel-math-kernel-library/topic/289702
#endif

/* _freebsd_cos(x)
 * Return cosine function of x.
 *
 * kernel function:
 *	__kernel_sin		... sine function on [-pi/4,pi/4]
 *	__kernel_cos		... cosine function on [-pi/4,pi/4]
 *	__ieee754_rem_pio2	... argument reduction routine
 *
 * Method.
 *      Let S,C and T denote the sin, cos and tan respectively on
 *	[-PI/4, +PI/4]. Reduce the argument x to y1+y2 = x-k*pi/2
 *	in [-pi/4 , +pi/4], and let n = k mod 4.
 *	We have
 *
 *          n        sin(x)      cos(x)        tan(x)
 *     ----------------------------------------------------------
 *	    0	       S	   C		 T
 *	    1	       C	  -S		-1/T
 *	    2	      -S	  -C		 T
 *	    3	      -C	   S		-1/T
 *     ----------------------------------------------------------
 *
 * Special cases:
 *      Let trig be any of sin, cos, or tan.
 *      trig(+-INF)  is NaN, with signals;
 *      trig(NaN)    is that NaN;
 *
 * Accuracy:
 *	TRIG(x) returns trig(x) nearly rounded
 */
EXTERN_C double bsd_cos(double x)
{
	PROLOGUE_DOUBLE();
	double y[2], z = 0.0;
	int32_t n, ix;

	/* High word of x. */
	GET_HIGH_WORD(ix, x);

	/* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if (ix <= 0x3fe921fb)
	{
		if (ix < 0x3e46a09e)			/* if x < 2**-27 * sqrt(2) */
			if (((int)x) == 0)
			{
				RETURN_DOUBLE (1.0);	/* generate inexact */
			}
		RETURN_DOUBLE (__kernel_cos(x, z));
	}

	/* cos(Inf or NaN) is NaN */
	else if (ix >= 0x7ff00000)
	{
		RETURN_DOUBLE (x - x);
	}

	/* argument reduction needed */
	else
	{
		n = __ieee754_rem_pio2(x, y);
		switch (n & 3)
		{
		case 0:
			RETURN_DOUBLE (__kernel_cos(y[0], y[1]));
		case 1:
			RETURN_DOUBLE (-__kernel_sin(y[0], y[1], 1));
		case 2:
			RETURN_DOUBLE (-__kernel_cos(y[0], y[1]));
		default:
			RETURN_DOUBLE (__kernel_sin(y[0], y[1], 1));
		}
	}
	EPILOGUE_DOUBLE();
}
