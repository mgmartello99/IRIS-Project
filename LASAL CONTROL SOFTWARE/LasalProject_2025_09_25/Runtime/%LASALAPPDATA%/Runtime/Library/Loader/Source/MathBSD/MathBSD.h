#ifdef LASAL_VERSION


(*
** General Functions
*)


// Computes the smallest integer value not less than x.
FUNCTION __CDECL GLOBAL bsd_ceil
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the smallest integer value not less than x.
FUNCTION __CDECL GLOBAL bsd_ceilf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the absolute value of a floating point value x.
FUNCTION __CDECL GLOBAL bsd_fabs
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the absolute value of a floating point value x.
FUNCTION __CDECL GLOBAL bsd_fabsf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the largest integer value not greater than x.
FUNCTION __CDECL GLOBAL bsd_fmod
	VAR_INPUT
		x : LREAL;
		y: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the absolute value of a floating point value x.
FUNCTION __CDECL GLOBAL bsd_fmodf
	VAR_INPUT
		x : REAL;
		y: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the largest integer value not greater than x.
FUNCTION __CDECL GLOBAL bsd_floor
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the absolute value of a floating point value x.
FUNCTION __CDECL GLOBAL bsd_floorf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the nearest integer value to x (in integer format), rounding halfway cases away from zero, regardless of the current rounding mode.
FUNCTION __CDECL GLOBAL bsd_lround
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : DINT;
	END_VAR;

// Computes the nearest integer value to x (in integer format), rounding halfway cases away from zero, regardless of the current rounding mode.
FUNCTION __CDECL GLOBAL bsd_lroundf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result : DINT;
	END_VAR;


// Decomposes given floating point value arg into integral and fractional parts, each having the same type and sign as x
FUNCTION __CDECL GLOBAL bsd_modf
	VAR_INPUT
		x : LREAL;
		iptr: ^LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Decomposes given floating point value arg into integral and fractional parts, each having the same type and sign as x
FUNCTION __CDECL GLOBAL bsd_modff
	VAR_INPUT
		x : REAL;
		iptr: ^REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the largest integer value not greater than x.
FUNCTION __CDECL GLOBAL bsd_remainder
	VAR_INPUT
		x : LREAL;
		y: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the absolute value of a floating point value x.
FUNCTION __CDECL GLOBAL bsd_remainderf
	VAR_INPUT
		x : REAL;
		y: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the floating-point remainder of the division operation x/y as the remainder() function does. Additionally, the sign and at least the three of the last bits of x/y will be stored in quo, sufficient to determine the octant of the result within a period.
FUNCTION __CDECL GLOBAL bsd_remquo
	VAR_INPUT
		x : LREAL;
		y: LREAL;
		quo: ^DINT;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the floating-point remainder of the division operation x/y as the remainder() function does. Additionally, the sign and at least the three of the last bits of x/y will be stored in quo, sufficient to determine the octant of the result within a period.
FUNCTION __CDECL GLOBAL bsd_remquof
	VAR_INPUT
		x : REAL;
		y: REAL;
		quo: ^DINT;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the nearest integer value to x, rounding halfway cases away from zero, regardless of the current rounding mode.
FUNCTION __CDECL GLOBAL bsd_round
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the nearest integer value to x, rounding halfway cases away from zero, regardless of the current rounding mode.
FUNCTION __CDECL GLOBAL bsd_roundf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the nearest integer not greater in magnitude than x.
FUNCTION __CDECL GLOBAL bsd_trunc
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the nearest integer not greater in magnitude than x.
FUNCTION __CDECL GLOBAL bsd_truncf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


(*
** Square Root and Related Functions
*)


// Computes the square root of the sum of the squares of x and y, without undue overflow or underflow
FUNCTION __CDECL GLOBAL bsd_hypot
	VAR_INPUT
		x : LREAL;
		y: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the square root of the sum of the squares of x and y, without undue overflow or underflow
FUNCTION __CDECL GLOBAL bsd_hypotf
	VAR_INPUT
		x : REAL;
		y: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes square root of x
FUNCTION __CDECL GLOBAL bsd_sqrt
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes square root of x
FUNCTION __CDECL GLOBAL bsd_sqrtf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


(*
** Trigonometric Functions
*)


// Computes the principal value of the arc cosine of x
FUNCTION __CDECL GLOBAL bsd_acos
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the principal value of the arc cosine of x
FUNCTION __CDECL GLOBAL bsd_acosf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;

// Computes the principal value of the arc sine of x
FUNCTION __CDECL GLOBAL bsd_asin
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the principal value of the arc sine of x
FUNCTION __CDECL GLOBAL bsd_asinf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the principal value of the arc tangent of x
FUNCTION __CDECL GLOBAL bsd_atan
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the principal value of the arc tangent of x
FUNCTION __CDECL GLOBAL bsd_atanf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the principal value of the arc tangent of x
FUNCTION __CDECL GLOBAL bsd_atan2
	VAR_INPUT
		y : LREAL;
		x: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the principal value of the arc tangent of x
FUNCTION __CDECL GLOBAL bsd_atan2f
	VAR_INPUT
		y : REAL;
		x: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the cosine of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_cos
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the cosine of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_cosf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the sine of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_sin
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the sine of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_sinf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the tangent of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_tan
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the tangent of x (measured in radians)
FUNCTION __CDECL GLOBAL bsd_tanf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Calculate sin and cos simultaneously
FUNCTION __CDECL GLOBAL bsd_sincos
	VAR_INPUT
		x : LREAL;
		sine: ^LREAL;
		cosine: ^LREAL;
	END_VAR;

// Calculate sin and cos simultaneously
FUNCTION __CDECL GLOBAL bsd_sincosf
	VAR_INPUT
		x : REAL;
		sine: ^REAL;
		cosine: ^REAL;
	END_VAR;


(*
** Hyperbolic Functions
*)


// Computes the inverse hyperbolic cosine of x.
FUNCTION __CDECL GLOBAL bsd_acosh
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the inverse hyperbolic cosine of x.
FUNCTION __CDECL GLOBAL bsd_acoshf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the inverse hyperbolic sine of x.
FUNCTION __CDECL GLOBAL bsd_asinh
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the inverse hyperbolic sine of x.
FUNCTION __CDECL GLOBAL bsd_asinhf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


//! Computes the inverse hyperbolic tangent of x.
FUNCTION __CDECL GLOBAL bsd_atanh
	VAR_INPUT
		x: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

//! Computes the inverse hyperbolic tangent of x.
FUNCTION __CDECL GLOBAL bsd_atanhf
	VAR_INPUT
		x: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the hyperbolic cosine of x
FUNCTION __CDECL GLOBAL bsd_cosh
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the hyperbolic cosine of x
FUNCTION __CDECL GLOBAL bsd_coshf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes hyperbolic sine of x
FUNCTION __CDECL GLOBAL bsd_sinh
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes hyperbolic sine of x
FUNCTION __CDECL GLOBAL bsd_sinhf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the hyperbolic tangent of x
FUNCTION __CDECL GLOBAL bsd_tanh
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the hyperbolic tangent of x
FUNCTION __CDECL GLOBAL bsd_tanhf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


(*
** Exponentiation and Logarithmic Functions
*)


// Computes the e (Euler's number, 2.7182818) raised to the given power x
FUNCTION __CDECL GLOBAL bsd_exp
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the e (Euler's number, 2.7182818) raised to the given power x
FUNCTION __CDECL GLOBAL bsd_expf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


//! Computes 2 raised to the given power x.
FUNCTION __CDECL GLOBAL bsd_exp2
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

//! Computes 2 raised to the given power x.
FUNCTION __CDECL GLOBAL bsd_exp2f
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the e (Euler's number, 2.7182818) raised to the given power x, minus 1.0. This function is more accurate than the expression exp(x)-1.0 if x is close to zero.
FUNCTION __CDECL GLOBAL bsd_expm1
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the e (Euler's number, 2.7182818) raised to the given power x, minus 1.0. This function is more accurate than the expression exp(x)-1.0 if x is close to zero.
FUNCTION __CDECL GLOBAL bsd_expm1f
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the natural (base e) logarithm of x.
FUNCTION __CDECL GLOBAL bsd_log
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the natural (base e) logarithm of x.
FUNCTION __CDECL GLOBAL bsd_logf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the natural (base e) logarithm of 1+x. This function is more precise than the expression log(1+x) if x is close to zero.
FUNCTION __CDECL GLOBAL bsd_log1p
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the natural (base e) logarithm of 1+x. This function is more precise than the expression log(1+x) if x is close to zero.
FUNCTION __CDECL GLOBAL bsd_log1pf
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the common (base-10) logarithm of x.
FUNCTION __CDECL GLOBAL bsd_log10
	VAR_INPUT
		x : LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the common (base-10) logarithm of x.
FUNCTION __CDECL GLOBAL bsd_log10f
	VAR_INPUT
		x : REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Computes the value of x raised to the power y.
FUNCTION __CDECL GLOBAL bsd_pow
	VAR_INPUT
		x : LREAL;
		y: LREAL;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Computes the value of x raised to the power y.
FUNCTION __CDECL GLOBAL bsd_powf
	VAR_INPUT
		x : REAL;
		y: REAL;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


(*
** Base Level Functions
*)


// Decomposes given floating point value x into a normalized fraction and an integral power of two. 
FUNCTION __CDECL GLOBAL bsd_frexp
	VAR_INPUT
		x : LREAL;
		eptr: ^DINT;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Decomposes given floating point value x into a normalized fraction and an integral power of two. 
FUNCTION __CDECL GLOBAL bsd_frexpf
	VAR_INPUT
		x : REAL;
		eptr: ^DINT;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


// Multiplies a floating point value x by 2 raised to power num (result := x * 2 ** num).
FUNCTION __CDECL GLOBAL bsd_scalbn
	VAR_INPUT
		x : LREAL;
		num: DINT;
	END_VAR
	VAR_OUTPUT
		result : LREAL;
	END_VAR;

// Multiplies a floating point value x by 2 raised to power num (result := x * 2 ** num).
FUNCTION __CDECL GLOBAL bsd_scalbnf
	VAR_INPUT
		x : REAL;
		num: DINT;
	END_VAR
	VAR_OUTPUT
		result(EAX) : REAL;
	END_VAR;


#else	// LASAL_VERSION


#pragma once

#ifdef _MSC_VER
#define FORCEINLINE		__forceinline
#else
#define FORCEINLINE		__attribute__((always_inline))
#endif

#ifdef __cplusplus
#define EXTERN_C	extern "C"
#else
#define EXTERN_C	extern
#endif

//! Maps float IEEE bts nto a opaque pointer to block implicit conversions, but use a register for return value
typedef void * LslFloatC_t;

// Lasal2 float calling convention
typedef union
{
	float v;
	/* FIXME: Assumes 32 bit int.  */
	LslFloatC_t ptr;
} L2F_t;

//! Extracts Lasal2 from opaque return type
inline float FORCEINLINE L2F_(LslFloatC_t lsl)
{
	L2F_t t;
	t.ptr = lsl;
	return t.v;
}

//! Converts float to opaque Lasal2 type
inline LslFloatC_t FORCEINLINE F2L_(float v)
{
	L2F_t t;
	t.v = v;
	return t.ptr;
}


/*
** General Functions
*/

//! Computes the smallest integer value not less than x.
EXTERN_C double bsd_ceil(double x);
//! Computes the smallest integer value not less than x.
EXTERN_C LslFloatC_t bsd_ceilf(float x);
//! Computes the absolute value of a floating point value x.
EXTERN_C double bsd_fabs(double x);
//! Computes the absolute value of a floating point value x.
EXTERN_C LslFloatC_t bsd_fabsf(float x);
//! Computes the largest integer value not greater than x.
EXTERN_C double bsd_floor(double x);
//! Computes the largest integer value not greater than x.
EXTERN_C LslFloatC_t bsd_floorf(float x);
//! Computes the floating-point remainder of the division operation x/y.
EXTERN_C double bsd_fmod(double x, double y);
//! Computes the floating-point remainder of the division operation x/y.
EXTERN_C LslFloatC_t bsd_fmodf(float x, float y);
//! Computes the nearest integer value to arg, rounding halfway cases away from zero, regardless of the current rounding mode.
EXTERN_C long bsd_lround(double x);
//! Computes the nearest integer value to arg, rounding halfway cases away from zero, regardless of the current rounding mode.
EXTERN_C long bsd_lroundf(float x);
//! Decomposes given floating point value arg into integral and fractional parts, each having the same type and sign as x
EXTERN_C double bsd_modf(double x, double *iptr);
//! Decomposes given floating point value arg into integral and fractional parts, each having the same type and sign as x
EXTERN_C LslFloatC_t bsd_modff(float x, float *iptr);
//! Computes the IEEE remainder of the floating point division operation x/y.
EXTERN_C double bsd_remainder(double x, double y);
//! Computes the IEEE remainder of the floating point division operation x/y.
EXTERN_C LslFloatC_t bsd_remainderf(float x, float y);
//! Computes the floating-point remainder of the division operation x/y as the remainder() function does. Additionally, the sign and at least the three of the last bits of x/y will be stored in quo, sufficient to determine the octant of the result within a period.
EXTERN_C double bsd_remquo(double x, double y, int *quo);
//! Computes the floating-point remainder of the division operation x/y as the remainder() function does. Additionally, the sign and at least the three of the last bits of x/y will be stored in quo, sufficient to determine the octant of the result within a period.
EXTERN_C LslFloatC_t bsd_remquof(float x, float y, int *quo);
//! Computes the nearest integer value to arg, rounding halfway cases away from zero, regardless of the current rounding mode.
EXTERN_C double bsd_round(double x);
//! Computes the nearest integer value to arg, rounding halfway cases away from zero, regardless of the current rounding mode.
EXTERN_C LslFloatC_t bsd_roundf(float x);
//! Computes the nearest integer not greater in magnitude than x.
EXTERN_C double bsd_trunc(double x);
//! Computes the nearest integer not greater in magnitude than x.
EXTERN_C LslFloatC_t bsd_truncf(float x);

/*
** Square Root and Related
*/

//! Computes the square root of the sum of the squares of x and y, without undue overflow or underflow
EXTERN_C double bsd_hypot(double x, double y);
//! Computes the square root of the sum of the squares of x and y, without undue overflow or underflow
EXTERN_C LslFloatC_t bsd_hypotf(float x, float y);
//! Computes square root of x
EXTERN_C double bsd_sqrt(double x);
//! Computes square root of x
EXTERN_C LslFloatC_t bsd_sqrtf(float x);

/*
** Trigonometric Functions
*/

//! Computes the principal value of the arc cosine of x
EXTERN_C double bsd_acos(double x);
//! Computes the principal value of the arc cosine of x
EXTERN_C LslFloatC_t bsd_acosf(float x);
//! Computes the principal value of the arc sine of x
EXTERN_C double bsd_asin(double x);
//! Computes the principal value of the arc sine of x
EXTERN_C LslFloatC_t bsd_asinf(float x);
//! Computes the principal value of the arc tangent of x
EXTERN_C double bsd_atan(double x);
//! Computes the principal value of the arc tangent of x
EXTERN_C LslFloatC_t bsd_atanf(float x);
//! Computes the arc tangent of y/x using the signs of arguments to determine the correct quadrant.
EXTERN_C double bsd_atan2(double y, double x);
//! Computes the arc tangent of y/x using the signs of arguments to determine the correct quadrant.
EXTERN_C LslFloatC_t bsd_atan2f(float y, float x);
//! Computes the cosine of x (measured in radians)
EXTERN_C double bsd_cos(double x);
//! Computes the cosine of x (measured in radians)
EXTERN_C LslFloatC_t bsd_cosf(float x);
//! Computes the sine of x (measured in radians)
EXTERN_C double bsd_sin(double x);
//! Computes the sine of x (measured in radians)
EXTERN_C LslFloatC_t bsd_sinf(float x);
//! Calculate sin and cos simultaneously
EXTERN_C void bsd_sincos(double x, double *sn, double *cs);
//! Calculate sin and cos simultaneously
EXTERN_C void bsd_sincosf(float x, float *sn, float *cs);
//! Computes the tangent of x (measured in radians)
EXTERN_C double bsd_tan(double x);
//! Computes the tangent of x (measured in radians)
EXTERN_C LslFloatC_t bsd_tanf(float x);

/*
** Hyperbolic Functions
*/

//! Computes the inverse hyperbolic cosine of x.
EXTERN_C double bsd_acosh(double x);
//! Computes the inverse hyperbolic cosine of x.
EXTERN_C LslFloatC_t bsd_acoshf(float x);
//! Computes the inverse hyperbolic sine of x.
EXTERN_C double bsd_asinh(double x);
//! Computes the inverse hyperbolic sine of x.
EXTERN_C LslFloatC_t bsd_asinhf(float x);
//! Computes the inverse hyperbolic tangent of x.
EXTERN_C double bsd_atanh(double x);
//! Computes the inverse hyperbolic tangent of x.
EXTERN_C LslFloatC_t bsd_atanhf(float x);
//! Computes the hyperbolic cosine of x
EXTERN_C double bsd_cosh(double x);
//! Computes the hyperbolic cosine of x
EXTERN_C LslFloatC_t bsd_coshf(float x);
//! Computes hyperbolic sine of x
EXTERN_C double bsd_sinh(double x);
//! Computes hyperbolic sine of x
EXTERN_C LslFloatC_t bsd_sinhf(float x);
//! Computes the hyperbolic tangent of x
EXTERN_C double bsd_tanh(double x);
//! Computes the hyperbolic tangent of x
EXTERN_C LslFloatC_t bsd_tanhf(float x);

/*
** Exponentiation and Logarithmic Functions
*/

//! Computes the e (Euler's number, 2.7182818) raised to the given power x
EXTERN_C double bsd_exp(double x);
//! Computes the e (Euler's number, 2.7182818) raised to the given power x
EXTERN_C LslFloatC_t bsd_expf(float x);
//! Computes 2 raised to the given power x.
EXTERN_C double bsd_exp2(double x);
//! Computes 2 raised to the given power x.
EXTERN_C LslFloatC_t bsd_exp2f(float x);
//! Computes the e (Euler's number, 2.7182818) raised to the given power x, minus 1.0. This function is more accurate than the expression exp(x)-1.0 if x is close to zero.
EXTERN_C double bsd_expm1(double x);
//! Computes the e (Euler's number, 2.7182818) raised to the given power x, minus 1.0. This function is more accurate than the expression exp(x)-1.0 if x is close to zero.
EXTERN_C LslFloatC_t bsd_expm1f(float x);
//! Computes the natural (base e) logarithm of x.
EXTERN_C double bsd_log(double x);
//! Computes the natural (base e) logarithm of x.
EXTERN_C LslFloatC_t bsd_logf(float x);
//! Computes the natural (base e) logarithm of 1+x. This function is more precise than the expression log(1+x) if x is close to zero.
EXTERN_C double bsd_log1p(double x);
//! Computes the natural (base e) logarithm of 1+x. This function is more precise than the expression log(1+x) if x is close to zero.
EXTERN_C LslFloatC_t bsd_log1pf(float x);
//! Computes the common (base-10) logarithm of x.
EXTERN_C double bsd_log10(double x);
//! Computes the common (base-10) logarithm of x.
EXTERN_C LslFloatC_t bsd_log10f(float x);
//! Computes the value of x raised to the power y
EXTERN_C double bsd_pow(double x, double y);
//! Computes the value of x raised to the power y
EXTERN_C LslFloatC_t bsd_powf(float x, float y);

/*
** Base Level Functions
*/

//! Decomposes given floating point value x into a normalized fraction and an integral power of two. 
EXTERN_C double bsd_frexp(double x, int *eptr);
//! Decomposes given floating point value x into a normalized fraction and an integral power of two. 
EXTERN_C LslFloatC_t bsd_frexpf(float x, int *eptr);
//! Multiplies a floating point value x by 2 raised to power n.
EXTERN_C double bsd_scalbn(double x, int n);
//! Multiplies a floating point value x by 2 raised to power n.
EXTERN_C LslFloatC_t bsd_scalbnf(float x, int n);


/*
**	These definitions are to extract Lasal2 float calling convention back to C data-type
*/

#define lsl_fabsf(x)			L2F_(bsd_fabsf(x))
#define lsl_atanf(x)			L2F_(bsd_atanf(x))
#define lsl_expm1f(x)			L2F_(bsd_expm1f(x))
#define lsl_expf(x)				L2F_(bsd_expf(x))
#define lsl_floorf(x)			L2F_(bsd_floorf(x))
#define lsl_fmodf(x, y)			L2F_(bsd_fmodf(x, y))
#define lsl_log1pf(x)			L2F_(bsd_log1pf(x))
#define lsl_logf(x)				L2F_(bsd_logf(x))
#define lsl_roundf(x)			L2F_(bsd_roundf(x))
#define lsl_sqrtf(x)			L2F_(bsd_sqrtf(x))
#define lsl_scalbnf(x, y)		L2F_(bsd_scalbnf(x, y))


#if 0

extern int signgam;

double	ldexp(double, int);

double	cbrt(double);
double	erf(double);
double	erfc(double);
double	fma(double, double, double);
int	ilogb(double);
double	lgamma(double);
double	log1p(double);
double	log2(double);
double	logb(double);
long	lrint(double);
double	nan(const char *);
double	nextafter(double, double);
double	rint(double);

double	j0(double);
double	j1(double);
double	jn(int, double);
double	y0(double);
double	y1(double);
double	yn(int, double);

double	gamma(double);

double	scalb(double, double);

double	fdim(double, double);
double	fmax(double, double);
double	fmin(double, double);
double	nearbyint(double);
double	scalbln(double, long);
double	tgamma(double);


double	drem(double, double);
int	finite(double);

double	gamma_r(double, int *);
double	lgamma_r(double, int *);

double	significand(double);



int	ilogbf(float);
float	ldexpf(float, int);
float	log1pf(float);
float	log2f(float);

float	erff(float);
float	erfcf(float);
float	lgammaf(float);
float	tgammaf(float);

float	cbrtf(float);
float	logbf(float);


long	lrintf(float);
float	rintf(float);
float	scalblnf(float, long);
float	fdimf(float, float);
float	fmaf(float, float, float);
float	fmaxf(float, float);
float	fminf(float, float);


// ??????????

float	nanf(const char *);
float	nearbyintf(float);
float	nextafterf(float, float);

#endif

#endif // LASAL_VERSION
