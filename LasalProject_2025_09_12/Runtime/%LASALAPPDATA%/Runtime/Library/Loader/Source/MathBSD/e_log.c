// <NewSigmatekCFileOptimize/>
/* @(#)e_log.c 1.3 95/01/18 */
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

// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/e_log.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_LOG_

asm(
	"	.global bsd_log; .type	bsd_log, %function;\n"
	"bsd_log:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	4(%esp) \n"
	"	movl	$0x127f, 0(%esp) \n"
	"	fldcw	0(%esp) \n"			// Apply new rounding mode

	"	fldln2 \n"
	"	fldl	12(%esp) \n"
	"	fyl2x \n"

	"	fldcw	4(%esp) \n"			// restore old rounding mode
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif

#else	// OPT_USE_FPU_LOG_


static const double
zero_D = 0.00000000000000000000e+00,	/* 0x00000000, 0x00000000 */
two54_D = 1.80143985094819840000e+16,	/* 0x43500000, 0x00000000 */
ln2_hi_D = 6.93147180369123816490e-01,	/* 0x3fe62e42, 0xfee00000 */
ln2_lo_D = 1.90821492927058770002e-10,	/* 0x3dea39ef, 0x35793c76 */
Lg1_D = 6.666666666666735130e-01,		/* 3FE55555 55555593 */
Lg2_D = 3.999999999940941908e-01,		/* 3FD99999 9997FA04 */
Lg3_D = 2.857142874366239149e-01,		/* 3FD24924 94229359 */
Lg4_D = 2.222219843214978396e-01,		/* 3FCC71C5 1D8E78AF */
Lg5_D = 1.818357216161805012e-01,		/* 3FC74664 96CB03DE */
Lg6_D = 1.531383769920937332e-01,		/* 3FC39A09 D078C69F */
Lg7_D = 1.479819860511658591e-01;		/* 3FC2F112 DF3E5244 */

static volatile double vzero_D = 0.0;


/* __ieee754_log(x)
 * Return the logrithm of x
 *
 * Method :                  
 *   1. Argument Reduction: find k and f such that 
 *			x = 2^k * (1+f), 
 *	   where  sqrt(2)/2 < 1+f < sqrt(2) .
 *
 *   2. Approximation of log(1+f).
 *	Let s = f/(2+f) ; based on log(1+f) = log(1+s) - log(1-s)
 *		 = 2s + 2/3 s**3 + 2/5 s**5 + .....,
 *	     	 = 2s + s*R
 *      We use a special Reme algorithm on [0,0.1716] to generate 
 * 	a polynomial of degree 14 to approximate R The maximum error 
 *	of this polynomial approximation is bounded by 2**-58.45. In
 *	other words,
 *		        2      4      6      8      10      12      14
 *	    R(z) ~ Lg1*s +Lg2*s +Lg3*s +Lg4*s +Lg5*s  +Lg6*s  +Lg7*s
 *  	(the values of Lg1 to Lg7 are listed in the program)
 *	and
 *	    |      2          14          |     -58.45
 *	    | Lg1*s +...+Lg7*s    -  R(z) | <= 2 
 *	    |                             |
 *	Note that 2s = f - s*f = f - hfsq + s*hfsq, where hfsq = f*f/2.
 *	In order to guarantee error in log below 1ulp, we compute log
 *	by
 *		log(1+f) = f - s*(f - R)	(if f is not too large)
 *		log(1+f) = f - (hfsq - s*(hfsq+R)).	(better accuracy)
 *	
 *	3. Finally,  log(x) = k*ln2 + log(1+f).  
 *			    = k*ln2_hi+(f-(hfsq-(s*(hfsq+R)+k*ln2_lo)))
 *	   Here ln2 is split into two floating point number: 
 *			ln2_hi + ln2_lo,
 *	   where n*ln2_hi is always exact for |n| < 2000.
 *
 * Special cases:
 *	log(x) is NaN with signal if x < 0 (including -INF) ; 
 *	log(+INF) is +INF; log(0) is -INF with signal;
 *	log(NaN) is that NaN with no signal.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */
EXTERN_C double bsd_log(double x)
{
	PROLOGUE_DOUBLE();
	double hfsq, f, s, z, R, w, t1, t2, dk;
	int32_t k, hx, i, j;
	u_int32_t lx;

	EXTRACT_WORDS(hx, lx, x);

	k = 0;
	if (hx < 0x00100000)
	{			/* x < 2**-1022  */
		if (((hx & 0x7fffffff) | lx) == 0)
		{
			RETURN_DOUBLE (- two54_D / vzero_D);		/* log(+-0)=-inf */
		}
		if (hx < 0)
		{
			RETURN_DOUBLE ((x - x) / zero_D);	/* log(-#) = NaN */
		}
		k -= 54; x *= two54_D; /* subnormal number, scale up x */
		GET_HIGH_WORD(hx, x);
	}
	if (hx >= 0x7ff00000)
	{
		RETURN_DOUBLE (x + x);
	}
	k += (hx >> 20) - 1023;
	hx &= 0x000fffff;
	i = (hx + 0x95f64) & 0x100000;
	SET_HIGH_WORD(x, hx | (i ^ 0x3ff00000));	/* normalize x or x/2 */
	k += (i >> 20);
	f = x - 1.0;
	if ((0x000fffff & (2 + hx)) < 3)
	{	/* -2**-20 <= f < 2**-20 */
		if (f == zero_D)
		{
			if (k == 0)
			{
				RETURN_DOUBLE (zero_D);
			}
			else
			{
				dk = (double)k;
				RETURN_DOUBLE (dk * ln2_hi_D + dk * ln2_lo_D);
			}
		}
		R = f * f*(0.5 - 0.33333333333333333*f);
		if (k == 0)
		{
			RETURN_DOUBLE (f - R);
		}
		else
		{
			dk = (double)k;
			RETURN_DOUBLE (dk * ln2_hi_D - ((R - dk * ln2_lo_D) - f));
		}
	}
	s = f / (2.0 + f);
	dk = (double)k;
	z = s * s;
	i = hx - 0x6147a;
	w = z * z;
	j = 0x6b851 - hx;
	t1 = w * (Lg2_D + w * (Lg4_D + w * Lg6_D));
	t2 = z * (Lg1_D + w * (Lg3_D + w * (Lg5_D + w * Lg7_D)));
	i |= j;
	R = t2 + t1;
	if (i > 0)
	{
		hfsq = 0.5*f*f;
		if (k == 0)
		{
			RETURN_DOUBLE (f - (hfsq - s * (hfsq + R)));
		}
		else
		{
			RETURN_DOUBLE (dk * ln2_hi_D - ((hfsq - (s*(hfsq + R) + dk * ln2_lo_D)) - f));
		}
	}
	else
	{
		if (k == 0)
		{
			RETURN_DOUBLE (f - s * (f - R));
		}
		else
		{
			RETURN_DOUBLE (dk * ln2_hi_D - ((s*(f - R) - dk * ln2_lo_D) - f));
		}
	}
	EPILOGUE_DOUBLE();
}

#endif	// OPT_USE_FPU_LOG_

