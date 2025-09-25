// <NewSigmatekCFileOptimize/>
/* @(#)s_scalbn.c 5.1 93/09/24 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_scalbn.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_SCALBN_

asm(
	"	.global bsd_scalbn; .type	bsd_scalbn, %function;\n"
	"	.global scalbn; .type	scalbn, %function;\n"
	"scalbn:\n"
	"bsd_scalbn:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	0(%esp) \n"
	"	movl	$0x127f, 4(%esp) \n"
	"	fldcw	4(%esp) \n"			// Apply new rounding mode

	"	fildl	20(%esp) \n"
	"	fldl	12(%esp) \n"
	"	fscale \n"
	"	fstp	%st(1) \n"

	"	fldcw	0(%esp) \n"			// restore old rounding mode
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_SCALBN_

static const double
two54_D = 1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54_D = 5.55111512312578270212e-17, /* 0x3C900000, 0x00000000 */
huge_D = 1.0e+300,
tiny_D = 1.0e-300;


/*
 * scalbn (double x, int n)
 * scalbn(x,n) returns x* 2**n  computed by  exponent
 * manipulation rather than by actually performing an
 * exponentiation or a multiplication.
 */
EXTERN_C double bsd_scalbn(double x, int n)
{
	PROLOGUE_DOUBLE();
	int32_t k, hx, lx;
	EXTRACT_WORDS(hx, lx, x);
	k = (hx & 0x7ff00000) >> 20;		/* extract exponent */
	if (k == 0)
	{				/* 0 or subnormal x */
		if ((lx | (hx & 0x7fffffff)) == 0)
		{
			RETURN_DOUBLE (x); /* +-0 */
		}
		x *= two54_D;
		GET_HIGH_WORD(hx, x);
		k = ((hx & 0x7ff00000) >> 20) - 54;
		if (n < -50000)
		{
			RETURN_DOUBLE (tiny_D * x); 	/*underflow*/
		}
	}
	if (k == 0x7ff)
	{
		RETURN_DOUBLE(x + x);		/* NaN or Inf */
	}
	k = k + n;
	if (k > 0x7fe)
	{
		RETURN_DOUBLE (huge_D * _bsd_copysign(huge_D, x)); /* overflow  */
	}
	if (k > 0) 				/* normal result */
	{
		SET_HIGH_WORD(x, (hx & 0x800fffff) | (k << 20));
		RETURN_DOUBLE (x);
	}
	if (k <= -54)
	{
		if (n > 50000) 	/* in case integer overflow in n+k */
		{
			RETURN_DOUBLE (huge_D * _bsd_copysign(huge_D, x));	/*overflow*/
		}
		else
		{
			RETURN_DOUBLE (tiny_D * _bsd_copysign(tiny_D, x)); 	/*underflow*/
		}
	}
	k += 54;				/* subnormal result */
	SET_HIGH_WORD(x, (hx & 0x800fffff) | (k << 20));
	RETURN_DOUBLE (x * twom54_D);
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double scalbn(double x, int n) ALIAS(bsd_scalbn);

#endif	// OPT_USE_FPU_SCALBN_
