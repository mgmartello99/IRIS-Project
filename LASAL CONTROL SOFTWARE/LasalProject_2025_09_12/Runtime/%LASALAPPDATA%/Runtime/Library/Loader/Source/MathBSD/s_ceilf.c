// <NewSigmatekCFileOptimize/>
/* s_ceilf.c -- float version of s_ceil.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_ceilf.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_CEILF_

asm(
	"	.global bsd_ceilf; .type	bsd_ceilf, %function;\n"
	"bsd_ceilf:\n"
);

#ifdef __arm__
#error No Assembly implementation for ARM CPU. Plz use Soft FP
#else
asm(
	"	pushl	%ebp \n"
	"	movl	%esp,%ebp \n"
	"	subl	$8,%esp \n"
	"	fstcw	-4(%ebp) \n"
	"	movw	-4(%ebp),%dx \n"
	"	orw		$0x0800,%dx \n"
	"	andw	$0xfbff,%dx \n"
	"	movw	%dx,-8(%ebp) \n"
	"	fldcw	-8(%ebp) \n"
	"	flds	8(%ebp) \n"
	"	frndint \n"
	"	fldcw	-4(%ebp) \n"

	"	fstps	-4(%esp) \n"
	"	movl	-4(%esp),%eax \n"
	"	leave \n"
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_CEILF_

static const float huge = 1.0e30f;

EXTERN_C LslFloatC_t bsd_ceilf(float x)
{
	PROLOGUE_FLOAT();
	int32_t i0, j0;
	u_int32_t i;

	GET_FLOAT_WORD(i0, x);
	j0 = ((i0 >> 23) & 0xff) - 0x7f;
	if (j0 < 23)
	{
		if (j0 < 0)
		{ 	/* raise inexact if x != 0 */
			if (huge + x > (float)0.0)
			{/* return 0*sign(x) if |x|<1 */
				if (i0 < 0) { i0 = 0x80000000; }
				else if (i0 != 0) { i0 = 0x3f800000; }
			}
		}
		else
		{
			i = (0x007fffff) >> j0;
			if ((i0&i) == 0)
			{
				RETURN_FLOAT(x); /* x is integral */
			}
			if (huge + x > (float)0.0)
			{	/* raise inexact flag */
				if (i0 > 0) i0 += (0x00800000) >> j0;
				i0 &= (~i);
			}
		}
	}
	else
	{
		if (j0 == 0x80)
		{
			RETURN_FLOAT(x + x);	/* inf or NaN */
		}
		else
		{
			RETURN_FLOAT(x);		/* x is integral */
		}
	}
	SET_FLOAT_WORD(x, i0);
	RETURN_FLOAT(x);
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_CEILF_

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float ceilf(float x) { return L2F_(bsd_ceilf(x)); }
