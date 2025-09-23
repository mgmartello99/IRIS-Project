// <NewSigmatekCFileOptimize/>
/* @(#)s_tan.c 5.1 93/09/24 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_tan.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_TAN_

asm(
	"	.global bsd_tan; .type	bsd_tan, %function;\n"
	"bsd_tan:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
#error The x86 FPU has significant deviations when using large x values
asm(
	"	fldl	4(%esp) \n"
	"	fptan \n"
	"	fnstsw	%ax \n"
	"	sahf \n"
	"	jp	1f \n"
	"	fstp	%st(0) \n"
	"	ret \n"
	"1:	fldpi \n"
	"	fadd	%st(0) \n"
	"	fxch	%st(1) \n"
	"2:	fprem1 \n"
	"	fnstsw	%ax \n"
	"	sahf \n"
	"	jp	2b \n"
	"	fstp	%st(1) \n"
	"	fptan \n"
	"	fstp	%st(0) \n"
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_TAN_

#include "e_rem_pio2.inl.h"


/* tan(x)
 * Return tangent function of x.
 *
 * kernel function:
 *	__kernel_tan		... tangent function on [-pi/4,pi/4]
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


#define INLINE_REM_PIO2

EXTERN_C double bsd_tan(double x)
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
		if (ix < 0x3e400000)				/* x < 2**-27 */
		{
			if ((int)x == 0)
			{
				RETURN_DOUBLE (x);		/* generate inexact */
			}
		}
		RETURN_DOUBLE (__kernel_tan(x, z, 1));
	}

	/* tan(Inf or NaN) is NaN */
	else if (ix >= 0x7ff00000)
	{
		RETURN_DOUBLE (x - x);		/* NaN */
	}

	/* argument reduction needed */
	else
	{
		n = __ieee754_rem_pio2(x, y);
		RETURN_DOUBLE (__kernel_tan(y[0], y[1], 1 - ((n & 1) << 1))); /*  1 -- n even
																-1 -- n odd */
	}
	EPILOGUE_DOUBLE();
}

#endif	// OPT_USE_FPU_TAN_
