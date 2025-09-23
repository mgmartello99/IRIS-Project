// <NewSigmatekCFileOptimize/>
/* e_remainderf.c -- float version of e_remainder.c.
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
 // https://svn.freebsd.org/base/head/lib/msun/src/e_remainderf.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_REMF_

asm(
	"	.global bsd_remainderf; .type	bsd_remainderf, %function;\n"
	"bsd_remainderf:\n"
);

#ifdef __arm__
#error No Assembly implementation for ARM CPU. Plz use Soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	0(%esp) \n"
	"	movl	$0x127f, 4(%esp) \n"
	"	fldcw	4(%esp) \n"			// Apply new rounding mode

	"	flds	16(%esp) \n"
	"	flds	12(%esp) \n"
	"1:	fprem1 \n"
	"	fstsw	%ax \n"
	"	sahf \n"
	"	jp	1b \n"
	"	fstp	%st(1) \n"

	"	fstps	4(%esp) \n"			// Convert to float
	"	movl	4(%esp), %eax \n"
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_REMF_

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static const float zero = 0.0;


EXTERN_C LslFloatC_t bsd_remainderf(float x, float p)
{
	PROLOGUE_FLOAT();
	int32_t hx, hp;
	u_int32_t sx;
	float p_half;

	GET_FLOAT_WORD(hx, x);
	GET_FLOAT_WORD(hp, p);
	sx = hx & 0x80000000;
	hp &= 0x7fffffff;
	hx &= 0x7fffffff;

	/* purge off exception values */
	if (hp == 0)
	{
		RETURN_FLOAT((x*p) / (x*p));	 	/* p = 0 */
	}
	if ((hx >= 0x7f800000) ||			/* x not finite */
		((hp > 0x7f800000)))			/* p is NaN */
	{
		RETURN_FLOAT((float)(((long double)x*p) / ((long double)x*p)));
	}


	if (hp <= 0x7effffff) x = lsl_fmodf(x, p + p);	/* now x < 2p */
	if ((hx - hp) == 0)
	{
		RETURN_FLOAT(zero * x);
	}
	x = lsl_fabsf(x);
	p = lsl_fabsf(p);
	if (hp < 0x01000000)
	{
		if (x + x > p)
		{
			x -= p;
			if (x + x >= p) x -= p;
		}
	}
	else
	{
		p_half = (float)0.5*p;
		if (x > p_half)
		{
			x -= p;
			if (x >= p_half) x -= p;
		}
	}
	GET_FLOAT_WORD(hx, x);
	if ((hx & 0x7fffffff) == 0) hx = 0;
	SET_FLOAT_WORD(x, hx^sx);
	RETURN_FLOAT(x);
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_REMF_

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float remainderf(float x, float p) { return L2F_(bsd_remainderf(x, p)); }
