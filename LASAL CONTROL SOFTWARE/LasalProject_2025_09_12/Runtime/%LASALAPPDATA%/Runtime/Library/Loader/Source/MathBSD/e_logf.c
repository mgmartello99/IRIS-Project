// <NewSigmatekCFileOptimize/>
/* e_logf.c -- float version of e_log.c.
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

// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/e_logf.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_LOGF_

asm(
	"	.global bsd_logf; .type	bsd_logf, %function;\n"
	"bsd_logf:\n"
);

#ifdef __arm__
#error No Assembly implementation for ARM CPU. Plz use Soft FP
#else
	asm(
		"	subl	$8, %esp \n"		// Storage for FPU CW
		"	fstcw	0(%esp) \n"
		"	movl	$0x127f, 4(%esp) \n"
		"	fldcw	4(%esp) \n"			// Apply new rounding mode

		"	fldln2 \n"
		"	flds	12(%esp) \n"
		"	fyl2x \n"

		"	fstps	4(%esp) \n"			// Convert to float
		"	movl	4(%esp), %eax \n"
		"	add		$8, %esp \n"		// Fix stack frame
		"	ret \n"
	);
#endif


#else	// OPT_USE_FPU_LOGF_


static const float
ln2_hi = 6.9313812256e-01f,	/* 0x3f317180 */
ln2_lo = 9.0580006145e-06f,	/* 0x3717f7d1 */
two25 = 3.355443200e+07f,	/* 0x4c000000 */
							/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
	Lg1 = 0xaaaaaa.0p-24f,	/* 0.66666662693 */
	Lg2 = 0xccce13.0p-25f,	/* 0.40000972152 */
	Lg3 = 0x91e9ee.0p-25f,	/* 0.28498786688 */
	Lg4 = 0xf89e26.0p-26f;	/* 0.24279078841 */

static const float zero = 0.0f;
static volatile float vzero = 0.0f;


EXTERN_C LslFloatC_t bsd_logf(float x)
{
	PROLOGUE_FLOAT();
	float hfsq, f, s, z, R, w, t1, t2, dk;
	int32_t k, ix, i, j;

	GET_FLOAT_WORD(ix, x);

	k = 0;
	if (ix < 0x00800000)
	{			/* x < 2**-126  */
		if ((ix & 0x7fffffff) == 0)
		{
			RETURN_FLOAT (-two25 / vzero);		/* log(+-0)=-inf */
		}
		if (ix < 0)
		{
			RETURN_FLOAT((x - x) / zero);	/* log(-#) = NaN */
		}
		k -= 25; x *= two25; /* subnormal number, scale up x */
		GET_FLOAT_WORD(ix, x);
	}
	if (ix >= 0x7f800000)
	{
		RETURN_FLOAT(x + x);
	}
	k += (ix >> 23) - 127;
	ix &= 0x007fffff;
	i = (ix + (0x95f64 << 3)) & 0x800000;
	SET_FLOAT_WORD(x, ix | (i ^ 0x3f800000));	/* normalize x or x/2 */
	k += (i >> 23);
	f = x - (float)1.0;
	if ((0x007fffff & (0x8000 + ix)) < 0xc000)
	{	/* -2**-9 <= f < 2**-9 */
		if (f == zero)
		{
			if (k == 0)
			{
				RETURN_FLOAT (zero);
			}
			else
			{
				dk = (float)k;
				RETURN_FLOAT (dk * ln2_hi + dk * ln2_lo);
			}
		}
		R = f * f*((float)0.5 - (float)0.33333333333333333*f);
		if (k == 0)
		{
			RETURN_FLOAT (f - R);
		}
		else
		{
			dk = (float)k;
			RETURN_FLOAT (dk * ln2_hi - ((R - dk * ln2_lo) - f));
		}
	}
	s = f / ((float)2.0 + f);
	dk = (float)k;
	z = s * s;
	i = ix - (0x6147a << 3);
	w = z * z;
	j = (0x6b851 << 3) - ix;
	t1 = w * (Lg2 + w * Lg4);
	t2 = z * (Lg1 + w * Lg3);
	i |= j;
	R = t2 + t1;
	if (i > 0)
	{
		hfsq = (float)0.5*f*f;
		if (k == 0)
		{
			RETURN_FLOAT(f - (hfsq - s * (hfsq + R)));
		}
		else
		{
			RETURN_FLOAT(dk * ln2_hi - ((hfsq - (s*(hfsq + R) + dk * ln2_lo)) - f));
		}
	}
	else
	{
		if (k == 0)
		{
			RETURN_FLOAT(f - s * (f - R));
		}
		else
		{
			RETURN_FLOAT(dk * ln2_hi - ((s*(f - R) - dk * ln2_lo) - f));
		}
	}
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_LOGF_
