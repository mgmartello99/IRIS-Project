// <NewSigmatekCFileOptimize/>
/* @(#)k_sin.c 1.3 95/01/18 */
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

 // Refactored from FreeBSD source code in
 // https://svn.freebsd.org/base/head/lib/msun/src/k_sin.c


#include "MathBSD.h"
#include "math_private.h"


static const double
half_D = 5.00000000000000000000e-01,	/* 0x3FE00000, 0x00000000 */
S1_D = -1.66666666666666324348e-01,	/* 0xBFC55555, 0x55555549 */
S2_D = 8.33333333332248946124e-03,	/* 0x3F811111, 0x1110F8A6 */
S3_D = -1.98412698298579493134e-04,	/* 0xBF2A01A0, 0x19C161D5 */
S4_D = 2.75573137070700676789e-06,	/* 0x3EC71DE3, 0x57B1FE7D */
S5_D = -2.50507602534068634195e-08,	/* 0xBE5AE5E6, 0x8A2B9CEB */
S6_D = 1.58969099521155010221e-10;	/* 0x3DE5D93A, 0x5ACFD57C */


/* __kernel_sin( x, y, iy)
 * kernel sin function on ~[-pi/4, pi/4] (except on -0), pi/4 ~ 0.7854
 * Input x is assumed to be bounded by ~pi/4 in magnitude.
 * Input y is the tail of x.
 * Input iy indicates whether y is 0. (if iy=0, y assume to be 0). 
 *
 * Algorithm
 *	1. Since sin(-x) = -sin(x), we need only to consider positive x. 
 *	2. Callers must return sin(-0) = -0 without calling here since our
 *	   odd polynomial is not evaluated in a way that preserves -0.
 *	   Callers may do the optimization sin(x) ~ x for tiny_D x.
 *	3. sin(x) is approximated by a polynomial of degree 13 on
 *	   [0,pi/4]
 *		  	         3            13
 *	   	sin(x) ~ x + S1_D*x + ... + S6_D*x
 *	   where
 *	
 * 	|sin(x)         2     4     6     8     10     12  |     -58
 * 	|----- - (1+S1_D*x +S2_D*x +S3_D*x +S4_D*x +S5_D*x  +S6_D*x   )| <= 2
 * 	|  x 					           | 
 * 
 *	4. sin(x+y) = sin(x) + sin'(x')*y
 *		    ~ sin(x) + (1-x*x/2)*y
 *	   For better accuracy, let 
 *		     3      2      2      2      2
 *		r = x *(S2_D+x *(S3_D+x *(S4_D+x *(S5_D+x *S6_D))))
 *	   then                   3    2
 *		sin(x) = x + (S1_D*x + (x *(r-y/2)+y))
 */
double __kernel_sin(double x, double y, int iy)
{
	double z, r, v, w;

	z = x * x;
	w = z * z;
	r = S2_D + z * (S3_D + z * S4_D) + z * w*(S5_D + z * S6_D);
	v = z * x;
	if (iy == 0) return x + v * (S1_D + z * r);
	else      return x - ((z*(half_D*y - v * r) - y) - v * S1_D);
}
