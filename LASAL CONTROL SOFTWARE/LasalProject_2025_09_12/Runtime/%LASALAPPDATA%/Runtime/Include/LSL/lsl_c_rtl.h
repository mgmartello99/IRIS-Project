#ifndef  __LSL_C_RTL_H
#pragma once
#define  __LSL_C_RTL_H

#if __GNUC__ >= 4

#include <math.h>
#include <Loader/MathBSD.h>


#	define abs				__builtin_abs
#	define acos(x)			bsd_acos(x)
#	define acosf(x)			L2F_(bsd_acosf(x))
#	define acosh(x)			bsd_acosh(x)
#	define acoshf(x)		L2F_(bsd_acoshf(x))
#	define asin(x)			bsd_asin(x)
#	define asinf(x)			L2F_(bsd_asinf(x))
#	define asinh(x)			bsd_asinh(x)
#	define asinhf(x)		L2F_(bsd_asinhf(x))
#	define atan(x)			bsd_atan(x)
#	define atan2(x, y)		bsd_atan2(x, y)
#	define atan2f(x, y)		L2F_(bsd_atan2f(x, y))
#	define atanf(x)			L2F_(bsd_atanf(x))
#	define atanh(x)			bsd_atanh(x)
#	define atanhf(x)		L2F_(bsd_atanhf(x))
#	define ceil(x)			bsd_ceil(x)
#	define ceilf(x)			L2F_(bsd_ceilf(x))
#	define copysign			__builtin_copysign
#	define copysignf		__builtin_copysignf
#	define cos(x)			bsd_cos(x)
#	define cosf(x)			L2F_(bsd_cosf(x))
#	define cosh(x)			bsd_cosh(x)
#	define coshf(x)			L2F_(bsd_coshf(x))
#	define exp(x)			bsd_exp(x)
#	define exp2(x)			bsd_exp2(x)
#	define exp2f(x)			L2F_(bsd_exp2f(x))
#	define expf(x)			L2F_(bsd_expf(x))
#	define expm1(x)			bsd_expm1(x)
#	define expm1f(x)		L2F_(bsd_expm1f(x))
#	define fabs				__builtin_fabs
#	define fabsf			__builtin_fabsf
#	define floor(x)			bsd_floor(x)
#	define floorf(x)		L2F_(bsd_floorf(x))
#	define fmod(x, y)		bsd_fmod(x, y)
#	define fmodf(x, y)		L2F_(bsd_fmodf(x, y))
#	define frexp(x)			bsd_frexp(x)
#	define frexpf(x)		L2F_(bsd_frexpf(x))
#	define hypot(x)			bsd_hypot(x)
#	define hypotf(x)		L2F_(bsd_hypotf(x))
#	define j0				_J0
#	define j1				_J1
#	define jn				_JN
#	define labs				__builtin_labs
#	define llabs			__builtin_llabs
#	define log(x)			bsd_log(x)
#	define log10(x)			bsd_log10(x)
#	define log10f(x)		L2F_(bsd_log10f(x))
#	define log1p(x)			bsd_log1p(x)
#	define log1pf(x)		L2F_(bsd_log1pf(x))
#	define logf(x)			L2F_(bsd_logf(x))
#	define lround(x)		bsd_lround(x)
#	define lroundf(x)		L2F_(bsd_lround(x))
#	define modf(x)			bsd_modf(x)
#	define modff(x)			L2F_(bsd_modff(x))
#	define pow(x, y)		bsd_pow(x, y)
#	define powf(x, y)		L2F_(bsd_powf(x, y))
#	define remainder(x, y)	bsd_remainder(x, y)
#	define remainderf(x, y)	L2F_(bsd_remainderf(x,y))
#	define remquo(x, y, q)	bsd_remquo(x, y, q)
#	define remquof(x, y, q)	L2F_(bsd_remquof(x, y, q))
#	define round(x)			bsd_round(x)
#	define roundf(x)		L2F_(bsd_roundf(x))
#	define scalbn(x, n)		bsd_scalbn(x, n)
#	define scalbnf(x, n)	bsd_scalbnf(x, n)
#	define sin(x)			bsd_sin(x)
#	define sincos(x, s, c)	bsd_sincos(x, s, c)
#	define sincosf(x, s, c)	bsd_sincosf(x, s, c)
#	define sinf(x)			L2F_(bsd_sinf(x))
#	define sinh(x)			bsd_sinh(x)
#	define sinhf(x)			L2F_(bsd_sinhf(x))
#	define sqrt(x)			bsd_sqrt(x)
#	define sqrtf(x)			L2F_(bsd_sqrtf(x))
#	define tan(x)			bsd_tan(x)
#	define tanf(x)			bsd_tanf(x)
#	define tanh(x)			bsd_tanh(x)
#	define tanhf(x)			L2F_(bsd_tanh(x))
#	define trunc(x)			bsd_trunc(x)
#	define truncf(x)		L2F_(bsd_truncf(x))
#	define y0				_Y0
#	define y1				_Y1
#	define yn				_YN
#	define _j0				_J0
#	define _j1				_J1
#	define _jn				_JN
#	define _y0				_Y0
#	define _y1				_Y1
#	define _yn				_YN

#else
	
#	define ftol FTOL
#	define abs ABS
#	define acos ACOS
#	define asin ASIN
#	define atan ATAN
#	define atan2 ATAN2
#	define cos COS
#	define cosh COSH
#	define exp EXP
#	define fabs FABS
#	define fmod FMOD
#	define labs LABS
#	define log LOG
#	define log10 LOG10
#	define pow POW
#	define sin SIN
#	define sinh SINH
#	define tan TAN
#	define tanh TANH
#	define sqrt SQRT
#	define atof ATOF
#	define ceil CEIL
#	define floor FLOOR
#	define frexp FREXP
#	define _hypot _HYPOT
#	define _j0 _J0
#	define _j1 _J1
#	define _jn _JN
#	define ldexp LDEXP
#	define modf MODF
#	define _y0 _Y0
#	define _y1 _Y1
#	define _yn _YN

#endif

//#include <RTOS_C_interfaces.h>

#endif // __LSL_C_RTL_H
