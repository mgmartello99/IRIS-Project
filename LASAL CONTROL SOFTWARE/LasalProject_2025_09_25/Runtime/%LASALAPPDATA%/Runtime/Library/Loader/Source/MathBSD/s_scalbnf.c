// <NewSigmatekCFileOptimize/>
/* s_scalbnf.c -- float version of s_scalbn.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_scalbnf.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_SCALBNF_

asm(
	"	.global bsd_scalbnf; .type	bsd_scalbnf, %function;\n"
	"bsd_scalbnf:\n"
);

#ifdef __arm__
#error No Assembly implementation for ARM CPU. Plz use Soft FP
#else
asm(
	"	fildl	8(%esp) \n"
	"	flds	4(%esp) \n"
	"	fscale \n"
	"	fstp	%st(1) \n"

	"	sub		$4, %esp \n"
	"	fstps	(%esp) \n"
	"	popl	%eax \n"
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_SCALBNF_

static const float
two25   =  3.355443200e+07f,	/* 0x4c000000 */
twom25  =  2.9802322388e-08f,	/* 0x33000000 */
huge   = 1.0e+30f,
tiny   = 1.0e-30f;

EXTERN_C LslFloatC_t bsd_scalbnf(float x, int n)
{
	PROLOGUE_FLOAT();
	int32_t k, ix;
	GET_FLOAT_WORD(ix, x);
	k = (ix & 0x7f800000) >> 23;		/* extract exponent */
	if (k == 0)
	{				/* 0 or subnormal x */
		if ((ix & 0x7fffffff) == 0)
		{
			RETURN_FLOAT(x); /* +-0 */
		}
		x *= two25;
		GET_FLOAT_WORD(ix, x);
		k = ((ix & 0x7f800000) >> 23) - 25;
		if (n < -50000)
		{
			RETURN_FLOAT(tiny * x); 	/*underflow*/
		}
	}
	if (k == 0xff)
	{
		RETURN_FLOAT(x + x);		/* NaN or Inf */
	}
	k = k + n;
	if (k > 0xfe)
	{
		RETURN_FLOAT(huge * _bsd_copysignf(huge, x)); /* overflow  */
	}
	if (k > 0) 				/* normal result */
	{
		SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
		RETURN_FLOAT(x);
	}
	if (k <= -25)
	{
		if (n > 50000) 	/* in case integer overflow in n+k */
		{
			RETURN_FLOAT(huge * _bsd_copysignf(huge, x));	/*overflow*/
		}
		else
		{
			RETURN_FLOAT(tiny * _bsd_copysignf(tiny, x));	/*underflow*/
		}
	}
	k += 25;				/* subnormal result */
	SET_FLOAT_WORD(x, (ix & 0x807fffff) | (k << 23));
	RETURN_FLOAT(x * twom25);
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_SCALBNF_

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float scalbnf(float x, int n) { return L2F_(bsd_scalbnf(x, n)); }
