#pragma once

#include "GetSetRound.h"

/*
**	This individually controls if a function is implemented in assembly/coprocessor
**	or with the BSD soft implementation (only where available)
*/

#if defined(_MSC_VER)
// Visual studio will always use the software implementation as it is the reference for the BSD implementation
#	define OPT_USE_FPU_CEIL_	0
#	define OPT_USE_FPU_CEILF_	0
#	define OPT_USE_FPU_COS_		0
#	define OPT_USE_FPU_COSF_	0
#	define OPT_USE_FPU_EXP_		0
#	define OPT_USE_FPU_EXPF_	0
#	define OPT_USE_FPU_FABS_	0
#	define OPT_USE_FPU_FABSF_	0
#	define OPT_USE_FPU_FLOOR_	0
#	define OPT_USE_FPU_FLOORF_	0
#	define OPT_USE_FPU_FMOD_	0
#	define OPT_USE_FPU_FMODF_	0
#	define OPT_USE_FPU_LOG_		0
#	define OPT_USE_FPU_LOGF_	0
#	define OPT_USE_FPU_LOG10_	0
#	define OPT_USE_FPU_LOG10F_	0
#	define OPT_USE_FPU_REM_		0
#	define OPT_USE_FPU_REMF_	0
#	define OPT_USE_FPU_SCALBN_	0
#	define OPT_USE_FPU_SCALBNF_	0
#	define OPT_USE_FPU_SIN_		0
#	define OPT_USE_FPU_SQRT_	0
#	define OPT_USE_FPU_SQRTF_	0
#	define OPT_USE_FPU_TAN_		0
#else
#	define OPT_USE_FPU_COS_		0
#	define OPT_USE_FPU_COSF_	0
#	define OPT_USE_FPU_EXPF_	0
#	define OPT_USE_FPU_FABS_	1
#	define OPT_USE_FPU_FABSF_	1
#	define OPT_USE_FPU_SQRT_	1
#	define OPT_USE_FPU_SQRTF_	1
#	define OPT_USE_FPU_TAN_		0
#	if defined(i386)
//		Intel FPU has far more features
#		define OPT_USE_FPU_CEIL_	1
#		define OPT_USE_FPU_CEILF_	1
#		define OPT_USE_FPU_EXP_		1
#		define OPT_USE_FPU_FLOOR_	1
#		define OPT_USE_FPU_FLOORF_	1
#		define OPT_USE_FPU_FMOD_	1
#		define OPT_USE_FPU_FMODF_	1
#		define OPT_USE_FPU_LOG_		1
#		define OPT_USE_FPU_LOGF_	1
#		define OPT_USE_FPU_LOG10_	1
#		define OPT_USE_FPU_LOG10F_	1
#		define OPT_USE_FPU_REM_		1
#		define OPT_USE_FPU_REMF_	1
#		define OPT_USE_FPU_SCALBN_	1
#		define OPT_USE_FPU_SCALBNF_	1
#		define OPT_USE_FPU_SIN_		1
#	else
#		define OPT_USE_FPU_CEIL_	0
#		define OPT_USE_FPU_CEILF_	0
#		define OPT_USE_FPU_EXP_		0
#		define OPT_USE_FPU_FLOOR_	0
#		define OPT_USE_FPU_FLOORF_	0
#		define OPT_USE_FPU_FMOD_	0
#		define OPT_USE_FPU_FMODF_	0
#		define OPT_USE_FPU_LOG_		0
#		define OPT_USE_FPU_LOGF_	0
#		define OPT_USE_FPU_LOG10_	0
#		define OPT_USE_FPU_LOG10F_	0
#		define OPT_USE_FPU_REM_		0
#		define OPT_USE_FPU_REMF_	0
#		define OPT_USE_FPU_SCALBN_	0
#		define OPT_USE_FPU_SCALBNF_	0
#		define OPT_USE_FPU_SIN_		0
#	endif
#endif

#ifndef M_PI_2
#	define M_PI_2     1.57079632679489661923
#endif

// Patch float return ABI for Lasal2 compatibility
/*
** This makes a copy of the return value into EAX/R0; this is required, because C 
** programs uses a co-processor register to return float values and Lasal2 compiler
** uses EAX/R0...
** This technique consists of using a void pointer as a opaque type associated to
** the IEE754 representation of the float value. C ABI returns void* using the
** regular EAX/R0 registers, exactly as Lasal2 implements floats. Many similar
** cast techniques where tried which caused diverse collateral effects, such as internal 
** compiler bugs, warning messages or differences between compiler tools. The best
** result was obtained by the use of the L2F_t union to switch between the two
** data types, without affecting data representation internally and with negligible
** performance penalty.
** Note that these technique makes the library compatible to both: the C and the 
** Lasal2 compilers.
*/
#if SAVE_FPU_STATE
#	define PROLOGUE_FLOAT()									\
		L2F_t ret_val__;									\
		uint32_t fpusave1__;								\
		uint32_t fpusave2__;								\
		__begin_mathbsd_fpu_context(fpusave1__, fpusave2__)
#	define RETURN_FLOAT(flt)						\
		{ ret_val__.v = (float)(flt); goto __exit_of_func__; }
#	define RETURN_L2F(l2f)							\
		{ ret_val__.ptr = l2f; goto __exit_of_func__; }
#	define EPILOGUE_FLOAT()							\
	__exit_of_func__:								\
		__end_mathbsd_fpu_context(fpusave1__);		\
		return ret_val__.ptr;
#	define PROLOGUE_DOUBLE()								\
		double ret_val__;									\
		uint32_t fpusave1__;								\
		uint32_t fpusave2__;								\
		__begin_mathbsd_fpu_context(fpusave1__, fpusave2__)
#	define RETURN_DOUBLE(flt)						\
		{ ret_val__ = (double)(flt); goto __exit_of_func__; }
#	define EPILOGUE_DOUBLE()						\
	__exit_of_func__:								\
		__end_mathbsd_fpu_context(fpusave1__);		\
		return ret_val__;
#	define PROLOGUE_VOID()								\
		uint32_t fpusave1__;								\
		uint32_t fpusave2__;								\
		__begin_mathbsd_fpu_context(fpusave1__, fpusave2__)
#	define RETURN_VOID()						\
		{ goto __exit_of_func__; }
#	define EPILOGUE_VOID()						\
	__exit_of_func__:								\
		__end_mathbsd_fpu_context(fpusave1__);		\
		return;
#else
#	define PROLOGUE_FLOAT()		L2F_t ret_val__
#	define RETURN_FLOAT(flt)						\
				{ ret_val__.v = (float)(flt); return ret_val__.ptr; }
#	define RETURN_L2F(l2f)		{ return l2f; }
#	define EPILOGUE_FLOAT()
#	define PROLOGUE_DOUBLE()
#	define RETURN_DOUBLE(val)	return val
#	define EPILOGUE_DOUBLE()
#	define PROLOGUE_VOID()
#	define RETURN_VOID()		return
#	define EPILOGUE_VOID()
#endif

#ifdef _MSC_VER
#	define DO_GCC_PRAGMA(x)
#else
#	define DO_GCC_PRAGMA(x) _Pragma (#x)
#endif

#ifdef __GNUC__

// Weak symbols are discarded if overwritten elsewhere
#define WEAK	__attribute__((weak))
// Assigns an existing symbol to the defined symbol
#define ALIAS(a)	__attribute__((alias(#a)))
// Assigns an existing symbol to the defined symbol
#define WEAKALIAS(a)	ALIAS(a) WEAK
//! Removes function prologue and epilogue code generation (i.e. removes push/pop and bx instructions)
#define NAKED		__attribute__((naked))
//! Marks a variable as unused and prevents warning messages
#define	UNUSED		__attribute__((unused))
//! Removes frame pointer of the function
#ifdef __arm__
#	define NO_FRAME_POINTER __attribute__((optimize("-fomit-frame-pointer")))
#else
#	define NO_FRAME_POINTER		// unsupported in x86 compiler
#endif

#endif


typedef signed int int32_t;
typedef unsigned int u_int32_t, uint32_t;
typedef unsigned long long u_int64_t;
typedef	float __float_t;
typedef	__float_t	float_t;


/*
 * A union which permits us to convert between a double and two 32 bit
 * ints.
 */
typedef union
{
	double value;
	struct
	{
		u_int32_t lsw;
		u_int32_t msw;
	} parts;
	struct
	{
		u_int64_t w;
	} xparts;
} ieee_double_shape_type;


/*
 * A union which permits us to convert between a float and a 32 bit
 * int.
 */
typedef union
{
	float value;
	/* FIXME: Assumes 32 bit int.  */
	unsigned int word;
} ieee_float_shape_type;


union IEEEf2bits
{
	float	f;
	struct
	{
		unsigned int	man : 23;
		unsigned int	exp : 8;
		unsigned int	sign : 1;
	} bits;
};


union IEEEd2bits
{
	double	d;
	struct
	{
		unsigned int	manl : 32;
		unsigned int	manh : 20;
		unsigned int	exp : 11;
		unsigned int	sign : 1;
	} bits;
};



/*
 * Attempt to get strict C99 semantics for assignment with non-C99 compilers.
 */
#define	STRICT_ASSIGN(type, lval, rval)	((lval) = (rval))

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(ix0,ix1,d)		\
	do {								\
		ieee_double_shape_type ew_u;	\
		ew_u.value = (d);				\
		(ix0) = ew_u.parts.msw;			\
		(ix1) = ew_u.parts.lsw;			\
	} while (0)

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(i,d)				\
	do {								\
		ieee_double_shape_type gh_u;	\
		gh_u.value = (d);				\
		(i) = gh_u.parts.msw;			\
	} while (0)

/* Get the less significant 32 bit int from a double.  */
#define GET_LOW_WORD(i,d)				\
	do {								\
		ieee_double_shape_type gl_u;	\
		gl_u.value = (d);				\
		(i) = gl_u.parts.lsw;			\
	} while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,ix0,ix1)			\
	do {								\
		ieee_double_shape_type iw_u;	\
		iw_u.parts.msw = (ix0);			\
		iw_u.parts.lsw = (ix1);			\
		(d) = iw_u.value;				\
	} while (0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,v)				\
	do {								\
		ieee_double_shape_type sh_u;	\
		sh_u.value = (d);				\
		sh_u.parts.msw = (v);			\
		(d) = sh_u.value;				\
	} while (0)

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(i,d)				\
	do {								\
		ieee_float_shape_type gf_u;		\
		gf_u.value = (d);				\
		(i) = gf_u.word;				\
	} while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d,i)				\
	do {								\
		ieee_float_shape_type sf_u;		\
		sf_u.word = (i);				\
		(d) = sf_u.value;				\
	} while (0)

/* Set the less significant 32 bits of a double from an int.  */
#define SET_LOW_WORD(d,v)				\
	do {								\
		ieee_double_shape_type sl_u;	\
		sl_u.value = (d);				\
		sl_u.parts.lsw = (v);			\
		(d) = sl_u.value;				\
	} while (0)


EXTERN_C double _bsd_copysign(double x, double y);
EXTERN_C float _bsd_copysignf(float x, float y);
EXTERN_C double __kernel_cos(double x, double y);
EXTERN_C double __kernel_sin(double x, double y, int iy);
EXTERN_C int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec);
EXTERN_C double __kernel_tan(double x, double y, int iy);
EXTERN_C double __ldexp_exp(double x, int expt);
EXTERN_C float __ldexp_expf(float x, int expt);

