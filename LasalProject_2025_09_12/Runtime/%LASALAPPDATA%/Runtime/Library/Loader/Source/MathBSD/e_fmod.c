// <NewSigmatekCFileOptimize/>

/* @(#)e_fmod.c 1.3 95/01/18 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/e_fmod.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_FMOD_

asm(
	"	.global bsd_fmod; .type	bsd_fmod, %function;\n"
	"	.global fmod; .type	fmod, %function;\n"
	"fmod:\n"
	"bsd_fmod:\n"
);

#ifdef __arm__
#error no Assembly implementation for ARM CPU; Plz use soft FP
#else
asm(
	"	subl	$8, %esp \n"		// Storage for FPU CW
	"	fstcw	0(%esp) \n"
	"	movl	$0x127f, 4(%esp) \n"
	"	fldcw	4(%esp) \n"			// Apply new rounding mode

	"	fldl	20(%esp) \n"
	"	fldl	12(%esp) \n"
	"1:	fprem \n"
	"	fstsw	%ax \n"
	"	sahf \n"
	"	jp	1b \n"

	"	fstp	%st(1) \n"
	"	add		$8, %esp \n"		// Fix stack frame
	"	ret \n"
);
#endif

#else	// OPT_USE_FPU_FMOD_

static const double one = 1.0, Zero[] = { 0.0, -0.0, };


/* 
 * __ieee754_fmod(x,y)
 * Return x mod y in exact arithmetic
 * Method: shift and subtract
 */
EXTERN_C double bsd_fmod(double x, double y)
{
	PROLOGUE_DOUBLE();
	int32_t n, hx, hy, hz, ix, iy, sx, i;
	u_int32_t lx, ly, lz;

	EXTRACT_WORDS(hx, lx, x);
	EXTRACT_WORDS(hy, ly, y);
	sx = hx & 0x80000000;		/* sign of x */
	hx ^= sx;		/* |x| */
	hy &= 0x7fffffff;	/* |y| */

	/* purge off exception values */
	if ((hy | ly) == 0 || (hx >= 0x7ff00000) ||	/* y=0,or x not finite */
		((hy | ((ly | -(int32_t)ly) >> 31)) > 0x7ff00000))	/* or y is NaN */
	{
		RETURN_DOUBLE ((x*y) / (x*y));
	}
	if (hx <= hy)
	{
		if ((hx < hy) || (lx < ly))
		{
			RETURN_DOUBLE (x);	/* |x|<|y| return x */
		}
		if (lx == ly)
		{
			RETURN_DOUBLE (Zero[(u_int32_t)sx >> 31]);	/* |x|=|y| return x*0*/
		}
	}

	/* determine ix = ilogb(x) */
	if (hx < 0x00100000)
	{	/* subnormal x */
		if (hx == 0)
		{
			for (ix = -1043, i = lx; i > 0; i <<= 1) ix -= 1;
		}
		else
		{
			for (ix = -1022, i = (hx << 11); i > 0; i <<= 1) ix -= 1;
		}
	}
	else ix = (hx >> 20) - 1023;

 /* determine iy = ilogb(y) */
	if (hy < 0x00100000)
	{	/* subnormal y */
		if (hy == 0)
		{
			for (iy = -1043, i = ly; i > 0; i <<= 1) iy -= 1;
		}
		else
		{
			for (iy = -1022, i = (hy << 11); i > 0; i <<= 1) iy -= 1;
		}
	}
	else iy = (hy >> 20) - 1023;

 /* set up {hx,lx}, {hy,ly} and align y to x */
	if (ix >= -1022)
		hx = 0x00100000 | (0x000fffff & hx);
	else
	{		/* subnormal x, shift x to normal */
		n = -1022 - ix;
		if (n <= 31)
		{
			hx = (hx << n) | (lx >> (32 - n));
			lx <<= n;
		}
		else
		{
			hx = lx << (n - 32);
			lx = 0;
		}
	}
	if (iy >= -1022)
		hy = 0x00100000 | (0x000fffff & hy);
	else
	{		/* subnormal y, shift y to normal */
		n = -1022 - iy;
		if (n <= 31)
		{
			hy = (hy << n) | (ly >> (32 - n));
			ly <<= n;
		}
		else
		{
			hy = ly << (n - 32);
			ly = 0;
		}
	}

	/* fix point fmod */
	n = ix - iy;
	while (n--)
	{
		hz = hx - hy; lz = lx - ly; if (lx < ly) hz -= 1;
		if (hz < 0) { hx = hx + hx + (lx >> 31); lx = lx + lx; }
		else
		{
			if ((hz | lz) == 0) 		/* return sign(x)*0 */
			{
				RETURN_DOUBLE (Zero[(u_int32_t)sx >> 31]);
			}
			hx = hz + hz + (lz >> 31); lx = lz + lz;
		}
	}
	hz = hx - hy; lz = lx - ly; if (lx < ly) hz -= 1;
	if (hz >= 0) { hx = hz; lx = lz; }

	/* convert back to floating value and restore the sign */
	if ((hx | lx) == 0) 			/* return sign(x)*0 */
	{
		RETURN_DOUBLE (Zero[(u_int32_t)sx >> 31]);
	}
	while (hx < 0x00100000)
	{		/* normalize x */
		hx = hx + hx + (lx >> 31); lx = lx + lx;
		iy -= 1;
	}
	if (iy >= -1022)
	{	/* normalize output */
		hx = ((hx - 0x00100000) | ((iy + 1023) << 20));
		INSERT_WORDS(x, hx | sx, lx);
	}
	else
	{		/* subnormal output */
		n = -1022 - iy;
		if (n <= 20)
		{
			lx = (lx >> n) | ((u_int32_t)hx << (32 - n));
			hx >>= n;
		}
		else if (n <= 31)
		{
			lx = (hx << (32 - n)) | (lx >> n); hx = sx;
		}
		else
		{
			lx = hx >> (n - 32); hx = sx;
		}
		INSERT_WORDS(x, hx | sx, lx);
		x *= one;		/* create necessary signal */
	}
	RETURN_DOUBLE (x);		/* exact output */
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double fmod(double x, double y) ALIAS(bsd_fmod);

#endif	// OPT_USE_FPU_FMOD_
