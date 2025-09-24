// <NewSigmatekCFileOptimize/>
/* @(#)s_truncf.c 5.1 93/09/24 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_truncf.c


#include "MathBSD.h"
#include "math_private.h"


static const float huge = 1.0e30F;


/*
 * truncf(x)
 * Return x rounded toward 0 to integral value
 * Method:
 *	Bit twiddling.
 * Exception:
 *	Inexact flag raised if x not equal to truncf(x).
 */
EXTERN_C LslFloatC_t bsd_truncf(float x)
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
			if (huge + x > 0.0F)		/* |x|<1, so return 0*sign(x) */
				i0 &= 0x80000000;
		}
		else
		{
			i = (0x007fffff) >> j0;
			if ((i0&i) == 0)
			{
				RETURN_FLOAT (x); /* x is integral */
			}
			if (huge + x > 0.0F)		/* raise inexact flag */
				i0 &= (~i);
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

// [DEVSW-1877, 8D-5889] grumat: <math.h> Functions are missing 
EXTERN_C float truncf(float x) { return L2F_(bsd_truncf(x)); }
