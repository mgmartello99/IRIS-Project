/* k_cosf.c -- float version of k_cos.c
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Debugged and optimized by Bruce D. Evans.
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
// https://svn.freebsd.org/base/head/lib/msun/src/k_cosf.c


#ifdef _MSC_VER
#define VAL_C0_F	(-0.499999997251031003120)
#define VAL_C1_F	(0.0416666233237390631894)
#define VAL_C2_F	(-0.00138867637746099294692)
#define VAL_C3_F	(0.0000243904487962774090654)
#else
#define VAL_C0_F	(-0x1ffffffd0c5e81.0p-54)
#define VAL_C1_F	(0x155553e1053a42.0p-57)
#define VAL_C2_F	(-0x16c087e80f1e27.0p-62)
#define VAL_C3_F	(0x199342e0ee5069.0p-68)
#endif

static const double
one_D = 1.00000000000000000000e+00, /* 0x3FF00000, 0x00000000 */
C0_F = VAL_C0_F,
C1_F = VAL_C1_F,
C2_F = VAL_C2_F,
C3_F = VAL_C3_F;

/* |cos(x) - c(x)| < 2**-34.1 (~[-5.37e-11, 5.295e-11]). */
static FORCEINLINE float __kernel_cosdf(double x)
{
	double r, w, z;

	/* Try to optimize for parallel evaluation as in k_tanf.c. */
	z = x * x;
	w = z * z;
	r = C2_F + z * C3_F;
	return (float)(((one_D + z * C0_F) + w * C1_F) + (w*z)*r);
}
