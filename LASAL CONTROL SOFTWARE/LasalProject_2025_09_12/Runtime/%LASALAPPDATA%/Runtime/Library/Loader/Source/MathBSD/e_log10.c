// <NewSigmatekCFileOptimize/>
/* @(#)e_log10.c 1.3 95/01/18 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_log10.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_LOG10_

asm(
	"	.global bsd_log10; .type	bsd_log10, %function;\n"
	"bsd_log10:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	4(%esp) \n"
	"	movl	$0x127f, 0(%esp) \n"
	"	fldcw	0(%esp) \n"			// Apply new rounding mode

	"	fldlg2 \n"
	"	fldl	12(%esp) \n"
	"	fyl2x \n"

	"	fldcw	4(%esp) \n"			// restore old rounding mode
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif

#else	// OPT_USE_FPU_LOG10_

#include "k_log.inl.h"


static const double
zero_D = 0.00000000000000000000e+00, /* 0x00000000, 0x00000000 */
two54_D = 1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
log10_2hi_D = 3.01029995663611771306e-01, /* 0x3FD34413, 0x509F6000 */
log10_2lo_D = 3.69423907715893078616e-13; /* 0x3D59FEF3, 0x11F12B36 */

static volatile double vzero_D = 0.0;


/*
 * Return the base 10 logarithm of x.  See e_log.c and k_log.h for most
 * comments.
 *
 *    log10(x) = (f - 0.5*f*f + k_log1p(f)) / ln10 + k * log10(2)
 * in not-quite-routine extra precision.
 */
EXTERN_C double bsd_log10(double x)
{
	PROLOGUE_DOUBLE();
	double f, hfsq, hi, lo, r, val_hi, val_lo, w, y, y2;
	int32_t i, k, hx;
	u_int32_t lx;

	EXTRACT_WORDS(hx, lx, x);

	k = 0;
	if (hx < 0x00100000)
	{			/* x < 2**-1022  */
		if (((hx & 0x7fffffff) | lx) == 0)
		{
			RETURN_DOUBLE (-two54_D / vzero_D);		/* log(+-0)=-inf */
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
	if (hx == 0x3ff00000 && lx == 0)
	{
		RETURN_DOUBLE (zero_D);			/* log(1) = +0 */
	}
	k += (hx >> 20) - 1023;
	hx &= 0x000fffff;
	i = (hx + 0x95f64) & 0x100000;
	SET_HIGH_WORD(x, hx | (i ^ 0x3ff00000));	/* normalize x or x/2 */
	k += (i >> 20);
	y = (double)k;
	f = x - 1.0;
	hfsq = 0.5*f*f;
	r = k_log1p(f);

	/* See e_log2.c for most details. */
	hi = f - hfsq;
	SET_LOW_WORD(hi, 0);
	lo = (f - hi) - hfsq + r;
	val_hi = hi * ivln10hi_D;
	y2 = y * log10_2hi_D;
	val_lo = y * log10_2lo_D + (lo + hi)*ivln10lo_D + lo * ivln10hi_D;

	/*
	 * Extra precision in for adding y*log10_2hi is not strictly needed
	 * since there is no very large cancellation near x = sqrt(2) or
	 * x = 1/sqrt(2), but we do it anyway since it costs little on CPUs
	 * with some parallelism and it reduces the error for many args.
	 */
	w = y2 + val_hi;
	val_lo += (y2 - w) + val_hi;
	val_hi = w;

	RETURN_DOUBLE (val_lo + val_hi);
	EPILOGUE_DOUBLE();
}


#endif	// OPT_USE_FPU_LOG10_
