// <NewSigmatekCFileOptimize/>
/* s_fabsf.c -- float version of s_fabs.c.
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

// Refactored from FreeBSD source code in
// https://svn.freebsd.org/base/head/lib/msun/src/s_fabsf.c


#include "MathBSD.h"
#include "math_private.h"

#if OPT_USE_FPU_FABSF_

asm(
	"	.global bsd_fabsf; .type	bsd_fabsf, %function;\n"
	"bsd_fabsf:\n"
);

#ifdef __arm__
	asm(
		"	fmsr	s15, r0 \n"
		"	fabss	s15, s15 \n"
		"	fmrs	r0, s15 \n"
		"	bx		lr \n"
	);
#else
	asm(
		"	flds	4(%esp) \n"
		"	fabs \n"
		"	sub		$4, %esp \n"
		"	fstps	(%esp) \n"
		"	pop		%eax \n"
		"	ret \n"
	);
#endif

#else	// OPT_USE_FPU_FABSF_

/*
 * fabsf(x) returns the absolute value of x.
 */
EXTERN_C LslFloatC_t bsd_fabsf(float x)
{
	L2F_t ret;
	u_int32_t ix;
	GET_FLOAT_WORD(ix, x);
	SET_FLOAT_WORD(x, ix & 0x7fffffff);
	ret.v = x;
	return ret.ptr;
}

#endif	// OPT_USE_FPU_FABSF_
