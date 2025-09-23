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
// https://svn.freebsd.org/base/head/lib/msun/src/k_logf.c

#ifdef _MSC_VER
#define VAL_LG1_F	(0.66666662693f)
#define VAL_LG2_F	(0.40000972152f)
#define VAL_LG3_F	(0.28498786688f)
#define VAL_LG4_F	(0.24279078841f)
#else
#define VAL_LG1_F	(0xaaaaaa.0p-24)
#define VAL_LG2_F	(0xccce13.0p-25)
#define VAL_LG3_F	(0x91e9ee.0p-25)
#define VAL_LG4_F	(0xf89e26.0p-26)
#endif


static const float
/* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
Lg1_F = VAL_LG1_F,	/* 0.66666662693 */
Lg2_F = VAL_LG2_F,	/* 0.40000972152 */
Lg3_F = VAL_LG3_F,	/* 0.28498786688 */
Lg4_F = VAL_LG4_F;	/* 0.24279078841 */


static FORCEINLINE float k_log1pf(float f)
{
	float hfsq,s,z,R,w,t1,t2;

	s = f/((float)2.0+f);
	z = s*s;
	w = z*z;
	t1= w*(Lg2_F+w*Lg4_F);
	t2= z*(Lg1_F+w*Lg3_F);
	R = t2+t1;
	hfsq=(float)0.5*f*f;
	return s*(hfsq+R);
}
