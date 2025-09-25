// <NewSigmatekCFileOptimize/>
/* @(#)e_exp.c 1.6 04/04/22 */
/*
 * ====================================================
 * Copyright (C) 2004 by Sun Microsystems, Inc. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

// Refactored from FreeBSD source code from
// https://svn.freebsd.org/base/head/lib/msun/src/e_exp.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_EXP_

asm(
	"	.global bsd_exp; .type	bsd_exp, %function;\n"
	"bsd_exp:\n"
);

#ifdef __arm__
#	error	No implementation for ARM compiler: use soft math
#else
asm(
	"	/* \n"
	"	* If x is +-Inf, then the subtraction would give Inf-Inf = NaN. \n"
	"	* Avoid this.  Also avoid it if x is NaN for convenience. \n"
	"	*/ \n"
	"	movl	8(%esp), %eax \n"
	"	andl	$0x7fffffff, %eax \n"
	"	cmpl	$0x7ff00000, %eax \n"
	"	jae	x_Inf_or_NaN \n"
	" \n"
	"	fldl	4(%esp) \n"
	" \n"
	"	/* \n"
	"	* Extended precision is needed to reduce the maximum error from \n"
	"	* hundreds of ulps to less than 1 ulp.  Switch to it if necessary. \n"
	"	* We may as well set the rounding mode to to-nearest and mask traps \n"
	"	* if we switch. \n"
	"	*/ \n"
	"	fstcw	4(%esp) \n"
//	"	movl	4(%esp), %eax \n"
//	"	andl	$0x0300, %eax \n"
//	"	cmpl	$0x0300, %eax		/* RC == 0 && PC == 3? */ \n"
//	"	je	1f			/* jump if mode is good */ \n"
	"	movl	$0x137f, 8(%esp) \n"
	"	fldcw	8(%esp) \n"
//	"1: \n"
	"	fldl2e \n"
	"	fmulp				/* x * log2(e) */ \n"
	"	fst	%st(1) \n"
	"	frndint				/* int(x * log2(e)) */ \n"
	"	fst	%st(2) \n"
	"	fsubrp				/* fract(x * log2(e)) */ \n"
	"	f2xm1				/* 2^(fract(x * log2(e))) - 1 */ \n"
	"	fld1 \n"
	"	faddp				/* 2^(fract(x * log2(e))) */ \n"
	"	fscale				/* e^x */ \n"
	"	fstp	%st(1) \n"
	"	je	1f \n"
	"	fldcw	4(%esp) \n"
	"1: \n"
	"	ret \n"
	"	 \n"
	"x_Inf_or_NaN : \n"
	"	/* \n"
	"	* Return 0 if x is -Inf.  Otherwise just return x; when x is Inf \n"
	"	* this gives Inf, and when x is a NaN this gives the same result \n"
	"	* as (x + x) (x quieted). \n"
	"	*/ \n"
	"	cmpl	$0xfff00000, 8(%esp) \n"
	"	jne	x_not_minus_Inf \n"
	"	cmpl	$0, 4(%esp) \n"
	"	jne	x_not_minus_Inf \n"
	"	fldz \n"
	"	ret \n"
	"	 \n"
	"	x_not_minus_Inf : \n"
	"	fldl	4(%esp) \n"
	"	ret \n"
);
#endif

#else	// OPT_USE_FPU_EXP_


#ifdef _MSC_VER
static const u_int64_t val_1p1023_ = 0x7FE0000000000000UL;
#define VAL_1P1023	*((double *)&val_1p1023_)
#else
#define VAL_1P1023	(0x1p1023)
#endif


static const double
one_D = 1.00000000000000000000e+00,				/* 0x3FF00000, 0x00000000 */
halF_D[2] = { 0.5,-0.5, },
o_threshold_D = 7.09782712893383973096e+02,		/* 0x40862E42, 0xFEFA39EF */
u_threshold_D = -7.45133219101941108420e+02,	/* 0xc0874910, 0xD52D3051 */
ln2HI_D[2]   ={ 6.93147180369123816490e-01,		/* 0x3fe62e42, 0xfee00000 */
				-6.93147180369123816490e-01,},	/* 0xbfe62e42, 0xfee00000 */
ln2LO_D[2]   ={ 1.90821492927058770002e-10,		/* 0x3dea39ef, 0x35793c76 */
				-1.90821492927058770002e-10,},	/* 0xbdea39ef, 0x35793c76 */
invln2_D =  1.44269504088896338700e+00,			/* 0x3ff71547, 0x652b82fe */
P1_D   =  1.66666666666666019037e-01,			/* 0x3FC55555, 0x5555553E */
P2_D   = -2.77777777770155933842e-03,			/* 0xBF66C16C, 0x16BEBD93 */
P3_D   =  6.61375632143793436117e-05,			/* 0x3F11566A, 0xAF25DE2C */
P4_D   = -1.65339022054652515390e-06,			/* 0xBEBBBD41, 0xC5D26BF1 */
P5_D   =  4.13813679705723846039e-08;			/* 0x3E663769, 0x72BEA4D0 */

static volatile double
huge_D = 1.0e+300,
twom1000_D = 9.33263618503218878990e-302;		/* 2**-1000=0x01700000,0*/


/* __ieee754_exp(x)
 * Returns the exponential of x.
 *
 * Method
 *   1. Argument reduction:
 *      Reduce x to an r so that |r| <= 0.5*ln2 ~ 0.34658.
 *	Given x, find r and integer k such that
 *
 *               x = k*ln2 + r,  |r| <= 0.5*ln2.  
 *
 *      Here r will be represented as r = hi-lo for better 
 *	accuracy.
 *
 *   2. Approximation of exp(r) by a special rational function on
 *	the interval [0,0.34658]:
 *	Write
 *	    R(r**2) = r*(exp(r)+1)/(exp(r)-1) = 2 + r*r/6 - r**4/360 + ...
 *      We use a special Remes algorithm on [0,0.34658] to generate 
 * 	a polynomial of degree 5 to approximate R. The maximum error 
 *	of this polynomial approximation is bounded by 2**-59. In
 *	other words,
 *	    R(z) ~ 2.0 + P1_D*z + P2_D*z**2 + P3_D*z**3 + P4_D*z**4 + P5_D*z**5
 *  	(where z=r*r, and the values of P1_D to P5_D are listed below)
 *	and
 *	    |                  5          |     -59
 *	    | 2.0+P1_D*z+...+P5_D*z   -  R(z) | <= 2 
 *	    |                             |
 *	The computation of exp(r) thus becomes
 *                             2*r
 *		exp(r) = 1 + -------
 *		              R - r
 *                                 r*R1(r)	
 *		       = 1 + r + ----------- (for better accuracy)
 *		                  2 - R1(r)
 *	where
 *			         2       4             10
 *		R1(r) = r - (P1_D*r  + P2_D*r  + ... + P5_D*r   ).
 *	
 *   3. Scale back to obtain exp(x):
 *	From step 1, we have
 *	   exp(x) = 2^k * exp(r)
 *
 * Special cases:
 *	exp(INF) is INF, exp(NaN) is NaN;
 *	exp(-INF) is 0, and
 *	for finite argument, only exp(0)=1 is exact.
 *
 * Accuracy:
 *	according to an error analysis, the error is always less than
 *	1 ulp (unit in the last place).
 *
 * Misc. info.
 *	For IEEE double 
 *	    if x >  7.09782712893383973096e+02 then exp(x) overflow
 *	    if x < -7.45133219101941108420e+02 then exp(x) underflow
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough
 * to produce the hexadecimal values shown.
 */
EXTERN_C double bsd_exp(double x)	/* default IEEE double exp */
{
	PROLOGUE_DOUBLE();
	double y, hi = 0.0, lo = 0.0, c, t, twopk;
	int32_t k = 0, xsb;
	u_int32_t hx;

	GET_HIGH_WORD(hx, x);
	xsb = (hx >> 31) & 1;		/* sign bit of x */
	hx &= 0x7fffffff;		/* high word of |x| */

	/* filter out non-finite argument */
	if (hx >= 0x40862E42)
	{			/* if |x|>=709.78... */
		if (hx >= 0x7ff00000)
		{
			u_int32_t lx;
			GET_LOW_WORD(lx, x);
			if (((hx & 0xfffff) | lx) != 0)
			{
				RETURN_DOUBLE (x + x); 		/* NaN */
			}
			else
			{
				RETURN_DOUBLE ((xsb == 0) ? x : 0.0);	/* exp(+-inf)={inf,0} */
			}
		}
		if (x > o_threshold_D)
		{
			RETURN_DOUBLE (huge_D * huge_D); /* overflow */
		}
		if (x < u_threshold_D)
		{
			RETURN_DOUBLE (twom1000_D * twom1000_D); /* underflow */
		}
	}

	/* argument reduction */
	if (hx > 0x3fd62e42)
	{		/* if  |x| > 0.5 ln2 */
		if (hx < 0x3FF0A2B2)
		{	/* and |x| < 1.5 ln2 */
			hi = x - ln2HI_D[xsb]; lo = ln2LO_D[xsb]; k = 1 - xsb - xsb;
		}
		else
		{
			k = (int)(invln2_D*x + halF_D[xsb]);
			t = k;
			hi = x - t * ln2HI_D[0];	/* t*ln2HI_D is exact here */
			lo = t * ln2LO_D[0];
		}
		STRICT_ASSIGN(double, x, hi - lo);
	}
	else if (hx < 0x3e300000)
	{	/* when |x|<2**-28 */
		if (huge_D + x > one_D)
		{
			RETURN_DOUBLE (one_D + x);/* trigger inexact */
		}
	}
	else k = 0;

	/* x is now in primary range */
	t = x * x;
	if (k >= -1021)
		INSERT_WORDS(twopk, 0x3ff00000 + (k << 20), 0);
	else
		INSERT_WORDS(twopk, 0x3ff00000 + ((k + 1000) << 20), 0);
	c = x - t * (P1_D + t * (P2_D + t * (P3_D + t * (P4_D + t * P5_D))));
	if (k == 0)
	{
		RETURN_DOUBLE (one_D - ((x*c) / (c - 2.0) - x));
	}
	else
		y = one_D - ((lo - (x*c) / (2.0 - c)) - hi);
	if (k >= -1021)
	{
		if (k == 1024)
		{
			RETURN_DOUBLE (y * 2.0*VAL_1P1023);
		}
		RETURN_DOUBLE (y * twopk);
	}
	else
	{
		RETURN_DOUBLE (y * twopk*twom1000_D);
	}
	EPILOGUE_DOUBLE();
}


#endif	// OPT_USE_FPU_EXP_
