// <NewSigmatekCFileOptimize/>
/* @(#)s_floor.c 5.1 93/09/24 */
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
 // https://svn.freebsd.org/base/head/lib/msun/src/s_floor.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_FLOOR_

asm(
	"	.global bsd_floor; .type	bsd_floor, %function;\n"
	"	.global floor; .type	floor, %function;\n"
	"floor:\n"
	"bsd_floor:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	pushl	%ebp \n"
	"	movl	%esp,%ebp \n"
	"	subl	$8,%esp \n"
	"	fstcw	-4(%ebp) \n"
	"	movw	-4(%ebp),%dx \n"
	"	orw		$0x0400,%dx \n"
	"	andw	$0xf7ff,%dx \n"
	"	movw	%dx,-8(%ebp) \n"
	"	fldcw	-8(%ebp) \n"
	"	fldl	8(%ebp) \n"
	"	frndint \n"
	"	fldcw	-4(%ebp) \n"
	"	leave \n"
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_FLOOR_

static const double huge_D = 1.0e+300;


/*
 * _freebsd_floor(x)
 * Return x rounded toward -inf to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to floor(x).
 */
EXTERN_C double bsd_floor(double x)
{
	PROLOGUE_DOUBLE();
	int32_t i0, i1, j0;
	u_int32_t i, j;
	EXTRACT_WORDS(i0, i1, x);
	j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
	if (j0 < 20)
	{
		if (j0 < 0)
		{ 	/* raise inexact if x != 0 */
			if (huge_D + x > 0.0)
			{/* return 0*sign(x) if |x|<1 */
				if (i0 >= 0) { i0 = i1 = 0; }
				else if (((i0 & 0x7fffffff) | i1) != 0)
				{
					i0 = 0xbff00000; i1 = 0;
				}
			}
		}
		else
		{
			i = (0x000fffff) >> j0;
			if (((i0&i) | i1) == 0)
			{
				RETURN_DOUBLE (x); /* x is integral */
			}
			if (huge_D + x > 0.0)
			{	/* raise inexact flag */
				if (i0 < 0) i0 += (0x00100000) >> j0;
				i0 &= (~i); i1 = 0;
			}
		}
	}
	else if (j0 > 51)
	{
		if (j0 == 0x400)
		{
			RETURN_DOUBLE (x + x);	/* inf or NaN */
		}
		else
		{
			RETURN_DOUBLE (x);		/* x is integral */
		}
	}
	else
	{
		i = ((u_int32_t)(0xffffffff)) >> (j0 - 20);
		if ((i1&i) == 0)
		{
			RETURN_DOUBLE (x);	/* x is integral */
		}
		if (huge_D + x > 0.0)
		{ 		/* raise inexact flag */
			if (i0 < 0)
			{
				if (j0 == 20) i0 += 1;
				else
				{
					j = i1 + (1 << (52 - j0));
					if ((int32_t)j < i1) i0 += 1; 	/* got a carry */
					i1 = j;
				}
			}
			i1 &= (~i);
		}
	}
	INSERT_WORDS(x, i0, i1);
	RETURN_DOUBLE (x);
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float floor(float x) ALIAS(bsd_floor);

#endif	// OPT_USE_FPU_FLOOR_
