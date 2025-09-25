// <NewSigmatekCFileOptimize/>

/* @(#)e_remainder.c 1.3 95/01/18 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_remainder.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_REM_

asm(
	"	.global bsd_remainder; .type	bsd_remainder, %function;\n"
	"	.global remainder; .type	remainder, %function;\n"
	"remainder:\n"
	"bsd_remainder:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	fldl	12(%esp) \n"
	"	fldl	4(%esp) \n"
	"1:	fprem1 \n"
	"	fstsw	%ax \n"
	"	sahf \n"
	"	jp	1b \n"
	"	fstp	%st(1) \n"
	"	ret \n"
);
#endif

#else	// OPT_USE_FPU_REM_

#ifdef __GNUC__
#define bsd_fabs(x)	__builtin_fabs(x)
#endif


static const double zero = 0.0;


/* __ieee754_remainder(x,p)
 * Return :                  
 * 	returns  x REM p  =  x - [x/p]*p as if in infinite 
 * 	precise arithmetic, where [x/p] is the (infinite bit) 
 *	integer nearest x/p (in half way case choose the even one).
 * Method : 
 *	Based on fmod() return x-[x/p]chopped*p exactlp.
 */
EXTERN_C double bsd_remainder(double x, double p)
{
	PROLOGUE_DOUBLE();
	int32_t hx, hp;
	u_int32_t sx, lx, lp;
	double p_half;

	EXTRACT_WORDS(hx, lx, x);
	EXTRACT_WORDS(hp, lp, p);
	sx = hx & 0x80000000;
	hp &= 0x7fffffff;
	hx &= 0x7fffffff;

	/* purge off exception values */
	if ((hp | lp) == 0)
	{
		RETURN_DOUBLE((x*p) / (x*p)); 	/* p = 0 */
	}
	if ((hx >= 0x7ff00000) ||			/* x not finite */
		((hp >= 0x7ff00000) &&			/* p is NaN */
		(((hp - 0x7ff00000) | lp) != 0)))
	{
		RETURN_DOUBLE(((long double)x*p) / ((long double)x*p));
	}


	if (hp <= 0x7fdfffff) x = bsd_fmod(x, p + p);	/* now x < 2p */
	if (((hx - hp) | (lx - lp)) == 0)
	{
		RETURN_DOUBLE (zero * x);
	}
	x = bsd_fabs(x);
	p = bsd_fabs(p);
	if (hp < 0x00200000)
	{
		if (x + x > p)
		{
			x -= p;
			if (x + x >= p) x -= p;
		}
	}
	else
	{
		p_half = 0.5*p;
		if (x > p_half)
		{
			x -= p;
			if (x >= p_half) x -= p;
		}
	}
	GET_HIGH_WORD(hx, x);
	if ((hx & 0x7fffffff) == 0) hx = 0;
	SET_HIGH_WORD(x, hx^sx);
	RETURN_DOUBLE (x);
	EPILOGUE_DOUBLE();
}

	// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
	EXTERN_C double remainder(double x, double p) ALIAS(bsd_remainder);

#endif	// OPT_USE_FPU_REM_
