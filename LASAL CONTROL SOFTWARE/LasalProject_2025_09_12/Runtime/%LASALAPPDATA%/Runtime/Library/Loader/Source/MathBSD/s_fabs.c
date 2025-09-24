// <NewSigmatekCFileOptimize/>
/* @(#)s_fabs.c 5.1 93/09/24 */
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
// https://svn.freebsd.org/base/head/lib/msun/src/s_fabs.c


#include "MathBSD.h"
#include "math_private.h"


#if OPT_USE_FPU_FABS_

asm(
	"	.global bsd_fabs; .type	bsd_fabs, %function;\n"
	"bsd_fabs:\n"
);

#ifdef __arm__
asm(
	"	fmdrr	d16, r0, r1 \n"
	"	fabsd	d16, d16 \n"
	"	fmrrd	r0, r1, d16 \n"
	"	bx	lr \n"
);
#else
asm(
	"	fldl	4(%esp) \n"
	"	fabs \n"
	"	ret \n"
);
#endif


#else	// OPT_USE_FPU_FABS_

/*
 * fabs(x) returns the absolute value of x.
 */
EXTERN_C double bsd_fabs(double x)
{
	u_int32_t high;
	GET_HIGH_WORD(high, x);
	SET_HIGH_WORD(x, high & 0x7fffffff);
	return x;
}

#endif	// OPT_USE_FPU_FABS_
