// <NewSigmatekCFileOptimize/>
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_log10f.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_LOG10F_

asm(
	"	.global bsd_log10f; .type	bsd_log10f, %function;\n"
	"bsd_log10f:\n"
);

#ifdef __arm__
#error No Assembly implementation for ARM CPU. Plz use Soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	0(%esp) \n"
	"	movl	$0x127f, 4(%esp) \n"
	"	fldcw	4(%esp) \n"			// Apply new rounding mode

	"	fldlg2 \n"
	"	flds	12(%esp) \n"
	"	fyl2x \n"

	"	fstps	4(%esp) \n"			// Convert to float
	"	movl	4(%esp), %eax \n"
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_LOG10F_

#include "k_logf.inl.h"


static const float
ivln10hi_F = 4.3432617188e-01f, /* 0x3ede6000 */
ivln10lo_F = -3.1689971365e-05f, /* 0xb804ead9 */
two25_F = 3.355443200e+07f,	/* 0x4c000000 */
log10_2hi_F = 3.0102920532e-01f, /* 0x3e9a2080 */
log10_2lo_F = 7.9034151668e-07f; /* 0x355427db */

static const float zero_F = 0.0;
static const float vzero_F = 0.0;


EXTERN_C LslFloatC_t bsd_log10f(float x)
{
	PROLOGUE_FLOAT();
	float f, hfsq, hi, lo, r, y;
	int32_t i, k, hx;

	GET_FLOAT_WORD(hx, x);

	k = 0;
	if (hx < 0x00800000)
	{			/* x < 2**-126  */
		if ((hx & 0x7fffffff) == 0)
		{
			RETURN_FLOAT (-two25_F / vzero_F);		/* log(+-0)=-inf */
		}
		if (hx < 0)
		{
			RETURN_FLOAT ((x - x) / zero_F);	/* log(-#) = NaN */
		}
		k -= 25; x *= two25_F; /* subnormal number, scale up x */
		GET_FLOAT_WORD(hx, x);
	}
	if (hx >= 0x7f800000)
	{
		RETURN_FLOAT (x + x);
	}
	if (hx == 0x3f800000)
	{
		RETURN_FLOAT (zero_F);			/* log(1) = +0 */
	}
	k += (hx >> 23) - 127;
	hx &= 0x007fffff;
	i = (hx + (0x4afb0d)) & 0x800000;
	SET_FLOAT_WORD(x, hx | (i ^ 0x3f800000));	/* normalize x or x/2 */
	k += (i >> 23);
	y = (float)k;
	f = x - (float)1.0;
	hfsq = (float)0.5*f*f;
	r = k_log1pf(f);

	/* See e_log2f.c and e_log2.c for details. */
	if (sizeof(float_t) > sizeof(float))
	{
		RETURN_FLOAT (
			(r - hfsq + f) * ((float_t)ivln10lo_F + ivln10hi_F) +
						y * ((float_t)log10_2lo_F + log10_2hi_F))
			;
	}
	hi = f - hfsq;
	GET_FLOAT_WORD(hx, hi);
	SET_FLOAT_WORD(hi, hx & 0xfffff000);
	lo = (f - hi) - hfsq + r;
	RETURN_FLOAT (
		y * log10_2lo_F + (lo + hi)*ivln10lo_F + lo * ivln10hi_F + hi * ivln10hi_F +
				y * log10_2hi_F)
		;
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_LOG10F_
