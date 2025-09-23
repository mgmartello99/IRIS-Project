// <NewSigmatekCFileOptimize/>
/* e_fmodf.c -- float version of e_fmod.c.
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_fmodf.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_FMODF_

asm(
	"	.global bsd_fmodf; .type	bsd_fmodf, %function;\n"
	"bsd_fmodf:\n"
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
	"1:	fprem \n"
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


#else	// OPT_USE_FPU_FMODF_

static const float one = 1.0f, Zero[] = { 0.0f, -0.0f, };

/*
 * __ieee754_fmodf(x,y)
 * Return x mod y in exact arithmetic
 * Method: shift and subtract
 */
EXTERN_C LslFloatC_t bsd_fmodf(float x, float y)
{
	PROLOGUE_FLOAT();
	int32_t n, hx, hy, hz, ix, iy, sx, i;

	GET_FLOAT_WORD(hx, x);
	GET_FLOAT_WORD(hy, y);
	sx = hx & 0x80000000;		/* sign of x */
	hx ^= sx;		/* |x| */
	hy &= 0x7fffffff;	/* |y| */

	/* purge off exception values */
	if (hy == 0 || (hx >= 0x7f800000) ||		/* y=0,or x not finite */
		(hy > 0x7f800000))			/* or y is NaN */
	{
		RETURN_FLOAT((x*y) / (x*y));
	}
	if (hx < hy)
	{
		RETURN_FLOAT(x);			/* |x|<|y| return x */
	}
	if (hx == hy)
	{
		RETURN_FLOAT(Zero[(u_int32_t)sx >> 31]);	/* |x|=|y| return x*0*/
	}

	/* determine ix = ilogb(x) */
	if (hx < 0x00800000)
	{	/* subnormal x */
		for (ix = -126, i = (hx << 8); i > 0; i <<= 1) ix -= 1;
	}
	else ix = (hx >> 23) - 127;

 /* determine iy = ilogb(y) */
	if (hy < 0x00800000)
	{	/* subnormal y */
		for (iy = -126, i = (hy << 8); i >= 0; i <<= 1) iy -= 1;
	}
	else iy = (hy >> 23) - 127;

 /* set up {hx,lx}, {hy,ly} and align y to x */
	if (ix >= -126)
		hx = 0x00800000 | (0x007fffff & hx);
	else
	{		/* subnormal x, shift x to normal */
		n = -126 - ix;
		hx = hx << n;
	}
	if (iy >= -126)
		hy = 0x00800000 | (0x007fffff & hy);
	else
	{		/* subnormal y, shift y to normal */
		n = -126 - iy;
		hy = hy << n;
	}

	/* fix point fmod */
	n = ix - iy;
	while (n--)
	{
		hz = hx - hy;
		if (hz < 0) { hx = hx + hx; }
		else
		{
			if (hz == 0) 		/* return sign(x)*0 */
			{
				RETURN_FLOAT(Zero[(u_int32_t)sx >> 31]);
			}
			hx = hz + hz;
		}
	}
	hz = hx - hy;
	if (hz >= 0) { hx = hz; }

	/* convert back to floating value and restore the sign */
	if (hx == 0) 			/* return sign(x)*0 */
	{
		RETURN_FLOAT(Zero[(u_int32_t)sx >> 31]);
	}
	while (hx < 0x00800000)
	{		/* normalize x */
		hx = hx + hx;
		iy -= 1;
	}
	if (iy >= -126)
	{		/* normalize output */
		hx = ((hx - 0x00800000) | ((iy + 127) << 23));
		SET_FLOAT_WORD(x, hx | sx);
	}
	else
	{		/* subnormal output */
		n = -126 - iy;
		hx >>= n;
		SET_FLOAT_WORD(x, hx | sx);
		x *= one;		/* create necessary signal */
	}
	RETURN_FLOAT(x);		/* exact output */
	EPILOGUE_FLOAT();
}

#endif	// OPT_USE_FPU_FMODF_

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float fmodf(float x, float y) { return L2F_(bsd_fmodf(x, y)); }
