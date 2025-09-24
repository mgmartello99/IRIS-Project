// <NewSigmatekCFileOptimize/>

/* @(#)e_atan2.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

 // Refactored from FreeBSD source code from
 // https://svn.freebsd.org/base/head/lib/msun/src/e_atan2.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#define bsd_fabs(x)	__builtin_fabs(x)
#endif


static volatile double
tiny = 1.0e-300;
static const double
zero = 0.0,
pi_o_4 = 7.8539816339744827900E-01, /* 0x3FE921FB, 0x54442D18 */
pi_o_2 = 1.5707963267948965580E+00, /* 0x3FF921FB, 0x54442D18 */
pi = 3.1415926535897931160E+00; /* 0x400921FB, 0x54442D18 */
static volatile double
pi_lo = 1.2246467991473531772E-16; /* 0x3CA1A626, 0x33145C07 */


/* __ieee754_atan2(y,x)
 * Method :
 *	1. Reduce y to positive by atan2(y,x)=-atan2(-y,x).
 *	2. Reduce x to positive by (if x and y are unexceptional): 
 *		ARG (x+iy) = arctan(y/x)   	   ... if x > 0,
 *		ARG (x+iy) = pi - arctan[y/(-x)]   ... if x < 0,
 *
 * Special cases:
 *
 *	ATAN2((anything), NaN ) is NaN;
 *	ATAN2(NAN , (anything) ) is NaN;
 *	ATAN2(+-0, +(anything but NaN)) is +-0  ;
 *	ATAN2(+-0, -(anything but NaN)) is +-pi ;
 *	ATAN2(+-(anything but 0 and NaN), 0) is +-pi/2;
 *	ATAN2(+-(anything but INF and NaN), +INF) is +-0 ;
 *	ATAN2(+-(anything but INF and NaN), -INF) is +-pi;
 *	ATAN2(+-INF,+INF ) is +-pi/4 ;
 *	ATAN2(+-INF,-INF ) is +-3pi/4;
 *	ATAN2(+-INF, (anything but,0,NaN, and INF)) is +-pi/2;
 *
 * Constants:
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */
EXTERN_C double bsd_atan2(double y, double x)
{
	PROLOGUE_DOUBLE();
	double z;
	int32_t k, m, hx, hy, ix, iy;
	u_int32_t lx, ly;

	EXTRACT_WORDS(hx, lx, x);
	ix = hx & 0x7fffffff;
	EXTRACT_WORDS(hy, ly, y);
	iy = hy & 0x7fffffff;
	if (((ix | ((lx | -(int32_t)lx) >> 31)) > 0x7ff00000) ||
		((iy | ((ly | -(int32_t)ly) >> 31)) > 0x7ff00000))	/* x or y is NaN */
	{
		RETURN_DOUBLE (x + y);
	}
	if (hx == 0x3ff00000 && lx == 0)
	{
		RETURN_DOUBLE (bsd_atan(y));   /* x=1.0 */
	}
	m = ((hy >> 31) & 1) | ((hx >> 30) & 2);	/* 2*sign(x)+sign(y) */

	/* when y = 0 */
	if ((iy | ly) == 0)
	{
		switch (m)
		{
		case 0:
		case 1:
			RETURN_DOUBLE (y); 	/* atan(+-0,+anything)=+-0 */
		case 2:
			RETURN_DOUBLE (pi + tiny);/* atan(+0,-anything) = pi */
		case 3:
			RETURN_DOUBLE (-pi - tiny);/* atan(-0,-anything) =-pi */
		}
	}
	/* when x = 0 */
	if ((ix | lx) == 0)
	{
		RETURN_DOUBLE ((hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny);
	}

	/* when x is INF */
	if (ix == 0x7ff00000)
	{
		if (iy == 0x7ff00000)
		{
			switch (m)
			{
			case 0:
				RETURN_DOUBLE (pi_o_4 + tiny);/* atan(+INF,+INF) */
			case 1:
				RETURN_DOUBLE (-pi_o_4 - tiny);/* atan(-INF,+INF) */
			case 2:
				RETURN_DOUBLE (3.0*pi_o_4 + tiny);/*atan(+INF,-INF)*/
			case 3:
				RETURN_DOUBLE (-3.0*pi_o_4 - tiny);/*atan(-INF,-INF)*/
			}
		}
		else
		{
			switch (m)
			{
			case 0:
				RETURN_DOUBLE (zero);	/* atan(+...,+INF) */
			case 1:
				RETURN_DOUBLE (-zero);	/* atan(-...,+INF) */
			case 2:
				RETURN_DOUBLE (pi + tiny);	/* atan(+...,-INF) */
			case 3:
				RETURN_DOUBLE (-pi - tiny);	/* atan(-...,-INF) */
			}
		}
	}
	/* when y is INF */
	if (iy == 0x7ff00000)
		RETURN_DOUBLE ((hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny);

	/* compute y/x */
	k = (iy - ix) >> 20;
	if (k > 60)
	{		 	/* |y/x| >  2**60 */
		z = pi_o_2 + 0.5*pi_lo;
		m &= 1;
	}
	else if (hx < 0 && k < -60) z = 0.0; 	/* 0 > |y|/x > -2**-60 */
	else z = bsd_atan(bsd_fabs(y / x));		/* safe to do y/x */
	switch (m)
	{
	case 0:
		RETURN_DOUBLE (z);	/* atan(+,+) */
	case 1:
		RETURN_DOUBLE (-z);	/* atan(-,+) */
	case 2:
		RETURN_DOUBLE (pi - (z - pi_lo));/* atan(+,-) */
	default: /* case 3 */
		RETURN_DOUBLE ((z - pi_lo) - pi);/* atan(-,-) */
	}
	EPILOGUE_DOUBLE();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C double atan2(double y, double x) ALIAS(bsd_atan2);
