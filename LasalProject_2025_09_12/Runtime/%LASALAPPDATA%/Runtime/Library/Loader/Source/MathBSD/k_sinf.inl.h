/* k_sinf.c -- float version of k_sin.c
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 * Optimized by Bruce D. Evans.
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
// https://svn.freebsd.org/base/head/lib/msun/src/k_sinf.c


#ifdef _MSC_VER
#define VAL_S1_F	(-0.166666666416265235595)
#define VAL_S2_F	(0.0083333293858894631756)
#define VAL_S3_F	(-0.000198393348360966317347)
#define VAL_S4_F	(0.0000027183114939898219064)
#else
#define VAL_S1_F	(-0x15555554cbac77.0p-55)
#define VAL_S2_F	(0x111110896efbb2.0p-59)
#define VAL_S3_F	(-0x1a00f9e2cae774.0p-65)
#define VAL_S4_F	(0x16cd878c3b46a7.0p-71)
#endif


static const double
S1_F = VAL_S1_F,
S2_F = VAL_S2_F,
S3_F = VAL_S3_F,
S4_F = VAL_S4_F;


static FORCEINLINE float __kernel_sindf(double x)
{
	double r, s, w, z;

	/* Try to optimize for parallel evaluation as in k_tanf.c. */
	z = x * x;
	w = z * z;
	r = S3_F + z * S4_F;
	s = z * x;
	return (float)((x + s * (S1_F + z * S2_F)) + s * w*r);
}

