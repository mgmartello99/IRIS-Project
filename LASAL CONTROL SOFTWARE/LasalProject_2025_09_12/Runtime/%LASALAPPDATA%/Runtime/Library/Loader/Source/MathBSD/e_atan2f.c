// <NewSigmatekCFileOptimize/>
/* e_atan2f.c -- float version of e_atan2.c.
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
 // https://svn.freebsd.org/base/head/lib/msun/src/e_atan2f.c


#include "MathBSD.h"
#include "math_private.h"

#ifdef __GNUC__
#ifdef lsl_fabsf
#undef lsl_fabsf
#endif
#define lsl_fabsf(x)	__builtin_fabsf(x)
#endif


static volatile float
tiny  = 1.0e-30f;
static const float
zero  = 0.0f,
pi_o_4  = 7.8539818525e-01f, /* 0x3f490fdb */
pi_o_2  = 1.5707963705e+00f, /* 0x3fc90fdb */
pi      = 3.1415927410e+00f; /* 0x40490fdb */
static volatile float
pi_lo   = -8.7422776573e-08f; /* 0xb3bbbd2e */

EXTERN_C LslFloatC_t bsd_atan2f(float y, float x)
{
	PROLOGUE_FLOAT();
	float z;
	int32_t k, m, hx, hy, ix, iy;

	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	GET_FLOAT_WORD(hy, y);
	iy = hy & 0x7fffffff;
	if ((ix > 0x7f800000) ||
		(iy > 0x7f800000))	/* x or y is NaN */
	{
		RETURN_FLOAT (x + y);
	}
	if (hx == 0x3f800000)
	{
		RETURN_L2F(bsd_atanf(y));   /* x=1.0 */
	}
	m = ((hy >> 31) & 1) | ((hx >> 30) & 2);	/* 2*sign(x)+sign(y) */

	/* when y = 0 */
	if (iy == 0)
	{
		switch (m)
		{
		case 0:
		case 1: RETURN_FLOAT (y); 	/* atan(+-0,+anything)=+-0 */
		case 2: RETURN_FLOAT (pi + tiny);/* atan(+0,-anything) = pi */
		case 3: RETURN_FLOAT (-pi - tiny);/* atan(-0,-anything) =-pi */
		}
	}
	/* when x = 0 */
	if (ix == 0)
	{
		RETURN_FLOAT ((hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny);
	}

	/* when x is INF */
	if (ix == 0x7f800000)
	{
		if (iy == 0x7f800000)
		{
			switch (m)
			{
			case 0: RETURN_FLOAT (pi_o_4 + tiny);/* atan(+INF,+INF) */
			case 1: RETURN_FLOAT (-pi_o_4 - tiny);/* atan(-INF,+INF) */
			case 2: RETURN_FLOAT ((float)3.0*pi_o_4 + tiny);/*atan(+INF,-INF)*/
			case 3: RETURN_FLOAT ((float)-3.0*pi_o_4 - tiny);/*atan(-INF,-INF)*/
			}
		}
		else
		{
			switch (m)
			{
			case 0: RETURN_FLOAT (zero);	/* atan(+...,+INF) */
			case 1: RETURN_FLOAT (-zero);	/* atan(-...,+INF) */
			case 2: RETURN_FLOAT (pi + tiny);	/* atan(+...,-INF) */
			case 3: RETURN_FLOAT (-pi - tiny);	/* atan(-...,-INF) */
			}
		}
	}
	/* when y is INF */
	if (iy == 0x7f800000)
	{
		RETURN_FLOAT ((hy < 0) ? -pi_o_2 - tiny : pi_o_2 + tiny);
	}

	/* compute y/x */
	k = (iy - ix) >> 23;
	if (k > 26)
	{			/* |y/x| >  2**26 */
		z = pi_o_2 + (float)0.5*pi_lo;
		m &= 1;
	}
	else if (k < -26 && hx < 0) z = 0.0; 	/* 0 > |y|/x > -2**-26 */
	else z = lsl_atanf(lsl_fabsf(y / x));	/* safe to do y/x */
	switch (m)
	{
	case 0: RETURN_FLOAT (z);	/* atan(+,+) */
	case 1: RETURN_FLOAT (-z);	/* atan(-,+) */
	case 2: RETURN_FLOAT (pi - (z - pi_lo));/* atan(+,-) */
	default: /* case 3 */
		RETURN_FLOAT ((z - pi_lo) - pi);/* atan(-,-) */
	}
	EPILOGUE_FLOAT();
}

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float atan2f(float y, float x) { return L2F_(bsd_atan2f(y, x)); }
