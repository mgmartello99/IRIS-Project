// <NewSigmatekCFileOptimize/>
/* s_cosf.c -- float version of s_cos.c.
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Optimized by Bruce D. Evans.
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


// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/s_cosf.c

#include "MathBSD.h"
#include "math_private.h"
#include "k_cosf.inl.h"
#include "k_sinf.inl.h"
#include "e_rem_pio2f.inl.h"


/* Small multiples of pi/2 rounded to double precision. */
static const double
s1pio2_D = 1 * M_PI_2,			/* 0x3FF921FB, 0x54442D18 */
s2pio2_D = 2 * M_PI_2,			/* 0x400921FB, 0x54442D18 */
s3pio2_D = 3 * M_PI_2,			/* 0x4012D97C, 0x7F3321D2 */
s4pio2_D = 4 * M_PI_2;			/* 0x401921FB, 0x54442D18 */


EXTERN_C LslFloatC_t bsd_cosf(float x)
{
	PROLOGUE_FLOAT();
	double y;
	int32_t n, hx, ix;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;

	if (ix <= 0x3f490fda)
	{		/* |x| ~<= pi/4 */
		if (ix < 0x39800000)		/* |x| < 2**-12 */
			if (((int)x) == 0)
			{
				RETURN_FLOAT (1.0);	/* 1 with inexact if x != 0 */
			}
		RETURN_FLOAT (__kernel_cosdf(x));
	}
	if (ix <= 0x407b53d1)
	{		/* |x| ~<= 5*pi/4 */
		if (ix > 0x4016cbe3)		/* |x|  ~> 3*pi/4 */
		{
			RETURN_FLOAT (-__kernel_cosdf(x + (hx > 0 ? -s2pio2_D : s2pio2_D)));
		}
		else
		{
			if (hx > 0)
			{
				RETURN_FLOAT (__kernel_sindf(s1pio2_D - x));
			}
			else
			{
				RETURN_FLOAT (__kernel_sindf(x + s1pio2_D));
			}
		}
	}
	if (ix <= 0x40e231d5)
	{		/* |x| ~<= 9*pi/4 */
		if (ix > 0x40afeddf)		/* |x|  ~> 7*pi/4 */
		{
			RETURN_FLOAT(__kernel_cosdf(x + (hx > 0 ? -s4pio2_D : s4pio2_D)));
		}
		else
		{
			if (hx > 0)
			{
				RETURN_FLOAT(__kernel_sindf(x - s3pio2_D));
			}
			else
			{
				RETURN_FLOAT(__kernel_sindf(-s3pio2_D - x));
			}
		}
	}

	/* cos(Inf or NaN) is NaN */
	else if (ix >= 0x7f800000)
	{
		RETURN_FLOAT (x - x);
	}

	/* general argument reduction needed */
	else
	{
		n = __ieee754_rem_pio2f(x, &y);
		switch (n & 3)
		{
		case 0: RETURN_FLOAT ( __kernel_cosdf(y));
		case 1: RETURN_FLOAT ( __kernel_sindf(-y));
		case 2: RETURN_FLOAT (-__kernel_cosdf(y));
		default:
			RETURN_FLOAT (__kernel_sindf(y));
		}
	}
	EPILOGUE_FLOAT();
}

