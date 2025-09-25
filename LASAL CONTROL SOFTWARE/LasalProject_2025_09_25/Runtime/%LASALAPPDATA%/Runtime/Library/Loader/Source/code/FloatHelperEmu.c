/********************************************************************************************
Floating Point Emulationssourcen
Mit dieser Kennung kann man die C-Compiler Optimierung explit einschalten.	
  <SigmatekCFile NoWarning/> <NewSigmatekCFileOptimize/>
********************************************************************************************/

// Intellisense stuff for syntax coloring
#ifdef __INTELLISENSE__
#	define _LSL_TARGETARCH_ARM
#endif

#include <stdint.h>
#include <float.h>
#include <limits.h>

// Definierung der Plattformabhängigen Funktionen
#include "../include/float_emu_config.h"


#if _OPTIMIZE_FLOAT_HELPER_EMU_ && defined(_LSL_TARGETARCH_ARM)
// Switches on optimizations for individual C functions
#	define OPTIMIZED __attribute__ ((optimize (3)))
// Optimizes with registers wherever used
#	define REGISTER  register
#else
// Switches off optimizations for individual C functions
#	define OPTIMIZED
// Skip register optimization
#	define REGISTER
#endif

#define NAKED __attribute__ ((naked, optimize (3)))

// Weak symbols are discarded if overwritten elsewhere
#define WEAK	__attribute__((weak))
// Assigns an existing symbol to the defined symbol
#define ALIAS(a)	__attribute__((alias(#a)))
// Assigns an existing symbol to the defined symbol
#define WEAKALIAS(a)	ALIAS(a) WEAK


typedef uint32_t UWtype;
typedef int32_t  Wtype;

struct DWstruct
{
	uint32_t low;
	int32_t high;
};

typedef union
{
	struct DWstruct s;
	int64_t ll;
} DWunion;


#define UINT32_MIN      0
#define UINT64_MIN      0


#ifdef _LSL_TARGETARCH_ARM
// This is an ARM instruction that throws a division error exception
#define THROW_DIV_EXCEPT() { asm volatile(".byte 0xF0, 0xF5, 0xF5, 0xF7\n" :::); }
// 32-bit CLZ (Count Leading Zeroes)
#define CLZ_32(r, v) { asm volatile("CLZ %[result], %[value]\n" : [result] "=r" (r) : [value] "r" (v): "cc"); }
// 64-bit CLZ (Count Leading Zeroes)
#define CLZ_64(r, hi, lo) \
{ \
	if (hi == 0) \
	{ \
		CLZ_32(r, lo); \
		r += 32; \
	} \
	else \
	{ \
		CLZ_32(r, hi); \
	} \
}
#endif


// Simple 32-bit division (using coprocessor is faster than softdiv in ARM CPU)
#define __udiv3232(r, lo, den)  (r = ((uint32_t)((double)lo / den)) )

// A label for the ranges arrays (g_LimitsU64 and g_LimitsDbl)
enum
{
	LIMIT_POSITIVE = 0, // matches bool sgn == false
	LIMIT_NEGATIVE = 1, // matches bool sgn == true
	LIMIT_UNSIGNED = 2,
};

#if ENABLE_DEAD_CODE

enum
{
	EExceptionNone,
	EExceptionDivZero,
	EExceptionDivOverflow,
	EExceptionSoftEmuAbort
};

#endif

#define abort   SoftEmuAbort

#ifdef __cplusplus
extern "C" {
#endif


// http://www.delorie.com/gnu/docs/gcc/gccint_53.html
// https://gcc.gnu.org/ml/gcc-help/2005-03/msg00087.html
typedef          long si_int;
typedef unsigned long su_int;
typedef          long long di_int, ti_int;
typedef unsigned long long du_int, tu_int;


#if defined(_USE_LLVM_SOFT_FLOAT)
// ********************************************************************************************
// Soft float implementation using LLVM source code
//		https://github.com/m-labs/compiler-rt-lm32
// ********************************************************************************************

typedef union
{
	di_int all;
	struct
	{
#if defined(ARCH_IS_LITTLE_ENDIAN)
		su_int low;
		si_int high;
#else
		si_int high;
		su_int low;
#endif /* defined(ARCH_IS_LITTLE_ENDIAN) */
	}s;
} dwords;


typedef union
{
	du_int all;
	struct
	{
#if defined(ARCH_IS_LITTLE_ENDIAN)
		su_int low;
		su_int high;
#else
		su_int high;
		su_int low;
#endif /* defined(ARCH_IS_LITTLE_ENDIAN) */
	}s;
} udwords;


typedef union
{
	su_int u;
	float f;
} float_bits;


typedef union
{
	udwords u;
	double  f;
} double_bits;



#if __SIZEOF_DOUBLE__ != 8
#	error "Code is designed for 64 bit doubles"
#endif

	typedef uint64_t rep_t;
	typedef int64_t srep_t;
	typedef double fp_t;
#define significandBits 52


#define typeWidth       (sizeof(rep_t)*8)
#define exponentBits    (typeWidth - significandBits - 1)
#define maxExponent     ((1 << exponentBits) - 1)
#define exponentBias    (maxExponent >> 1)
#define implicitBit     (1ULL << significandBits)
#define significandMask (implicitBit - 1U)
#define signBit         (1ULL << (significandBits + exponentBits))
#define absMask         (signBit - 1U)
#define exponentMask    (absMask ^ significandMask)
#define infRep          exponentMask


	static inline rep_t toRep(fp_t x) {
		const union { fp_t f; rep_t i; } rep = { x };
		return rep.i;
	}

#endif	// defined(_USE_LLVM_SOFT_FLOAT)


// ********************************************************************************************
// Sonder Funktionen
// ********************************************************************************************

/********************************************************************************************
  Manche Prozessoren haben keinen Divisionsbefehl,
  dafür gibt es diese Funktionen
********************************************************************************************/
#define divnorm(num, den, sign) 		\
{							\
	if (num < 0) 			\
	{						\
		num = -num;			\
		sign = 1;			\
	}						\
	else 					\
	{						\
		sign = 0;			\
	}						\
							\
	if (den < 0) 			\
	{						\
		den = - den;		\
		sign = 1 - sign;	\
	} 						\
}


#define exitdiv(sign, res)	return sign ? -res : res;

#if defined(_USE_MULDIV6432)

// Table of allowed limits
static uint64_t g_LimitsU64[] =
{
	2147483647ull,
	2147483648ull,
	4294967295ull,
};
// Table of allowed limits
static double g_LimitsDbl[] =
{
	2147483648.0,
	2147483649.0,
	4294967296.0,
};

// This routine implements the "Improved shift divisor method" using ARM CLZ and LSL 
// assembler instructions
// see https://www.microsoft.com/en-us/research/publication/software-integer-division/
uint32_t OPTIMIZED __udiv6432_soft_asm(uint32_t xlo, uint32_t xhi, uint32_t denominator, uint32_t sgn)
{
	REGISTER uint64_t q = 0;

	if (xhi || xlo >= denominator)
	{
		// Count leading zeroes
		REGISTER uint32_t lzx_cnt, lzy_cnt;
		CLZ_64(lzx_cnt, xhi, xlo);
		CLZ_32(lzy_cnt, denominator);
		lzy_cnt = lzy_cnt + 32 - lzx_cnt;

		REGISTER uint64_t y = (uint64_t)denominator << lzy_cnt;
		REGISTER uint64_t num = ((uint64_t)xhi << 32) | xlo;

		for (;;)
		{
			if (num >= y)
			{
				num -= y;
				++q;
			}
			if (lzy_cnt == 0)
				break;
			--lzy_cnt;
			q += q;
			y >>= 1;
		}
	}
	// Throw exception on purpose for an overflow
	if (q > g_LimitsU64[sgn])
		THROW_DIV_EXCEPT();

	return (uint32_t)q;
}

uint32_t OPTIMIZED __udivdi6432(uint32_t lo, uint32_t hi, uint32_t den)
{
	if (den == 0)
		THROW_DIV_EXCEPT();

	// Simple 32 bit division uses VFP coprocessor
	if (hi == 0)
	{
		REGISTER uint32_t ret;
		__udiv3232(ret, lo, den);
		return ret;
	}
	else
	{
		// Very large values will lose precision due to size of VFP registers
		if (hi >= 0x200000UL)
			return __udiv6432_soft_asm(lo, hi, den, LIMIT_UNSIGNED); // Use soft division

		// Convert to double precision and perform division using VFP
		double xx = (((double)hi * 65536.0*65536.0) + lo) / den;

		// Throw exception on purpose for an overflow
		if (xx >= g_LimitsDbl[LIMIT_UNSIGNED])
			THROW_DIV_EXCEPT();

		return (uint32_t)xx;
	}
}

int32_t OPTIMIZED __divdi6432(uint32_t lo, uint32_t hi, uint32_t den)
{
	bool sgn = false;

	if (den == 0)
		THROW_DIV_EXCEPT();

	// BLOCK: v scope
	{
		// Remove signs of value
		int64_t v = (int64_t)(((uint64_t)hi << 32) + lo);
		if ((int32_t)hi < 0)
		{
			sgn = true;
			v = -v;
		}
		// Remove signs of denominator
		if ((int32_t)den < 0)
		{
			sgn = !sgn;
			den = (uint32_t)(-((int32_t)den));  // 2's complement
		}
		lo = (uint32_t)(v);
		hi = (uint32_t)(v >> 32);
	}
	REGISTER uint32_t ret;
	// Use unsigned math
	if (hi == 0)
	{
		__udiv3232(ret, lo, den);
		if (ret > g_LimitsU64[sgn])
			THROW_DIV_EXCEPT();
	}
	// Very large values will loose precision due to size of VFP registers
	else if (hi >= 0x200000UL)
		ret = __udiv6432_soft_asm(lo, hi, den, sgn); // Use soft division
	else
	{
		// Convert to double precision and perform division using VFP
		double xx = (((double)hi * 65536.0*65536.0) + lo) / den;

				// Throw exception on purpose for an overflow
		if (xx >= g_LimitsDbl[sgn])
			THROW_DIV_EXCEPT();
		ret = (uint32_t)xx;
	}
	// Apply sign
	return (sgn) ? -(int32_t)ret : (int32_t)ret;
}


// These method is required by Lib.awl
uint32_t OPTIMIZED FloatHelperEmu_mod_udivdi6432(uint32_t* modRes, uint32_t lo, uint32_t hi, uint32_t denominator)
{
	register uint64_t num = hi;
	num = (num << 32) | lo;
	register uint64_t den = denominator;
	register uint64_t bit = 1;
	register uint64_t res = 0;

	while (den < num && bit && !(den & (1LL << 63L)))
	{
		den <<= 1;
		bit <<= 1;
	}
	while (bit)
	{
		if (num >= den)
		{
			num -= den;
			res |= bit;
		}
		bit >>= 1;
		den >>= 1;
	}
	if (modRes != 0)
		*modRes = (uint32_t)num;
	return res;
}

// These method is required by Lib.awl
int32_t OPTIMIZED FloatHelperEmu_mod_divdi6432(int32_t* modRes, uint32_t lo, uint32_t hi, uint32_t denominator)
{
	/*
  ** [8D-5014] MOD with negative values not agreeing with X86 results
	** https://en.wikipedia.org/wiki/Modulo_operation
	** Implementation of the *truncated division* (case 1)
	** Note that Windows Calculator uses the Euclidean modulo
	*/
	bool sgn = false;
	register uint64_t num;
	// BLOCK: v scope
	{
		// Remove sign of value
		int64_t v = (int64_t)(((uint64_t)hi << 32) + lo);
		if ((int32_t)hi < 0)
		{
			sgn = true;
			v = -v;
		}
		// Remove sign of denominator
		if ((int32_t)denominator < 0)
		{
			sgn = !sgn;
			denominator = (uint32_t)(-((int32_t)denominator));  // 2's complement
		}
		// Now everything is unsigned
		num = v;
	}
	
	register uint64_t den = denominator;
	register uint64_t bit = 1;
	register uint64_t res = 0;

	while (den < num && bit && !(den & (1LL << 63L)))
	{
		den <<= 1;
		bit <<= 1;
	}
	while (bit)
	{
		if (num >= den)
		{
			num -= den;
			res |= bit;
		}
		bit >>= 1;
		den >>= 1;
	}
	if (modRes != 0)
	{
		// Remainder has the same sign as dividend
		*modRes = ((int32_t)hi < 0) ? -(int32_t)num : (int32_t)num;
	}
	return sgn ? -(int32_t)res : (int32_t)res;
}
#endif // defined(_USE_MULDIV6432)


#if _USE_SOFT_I32_DIV == IFP_IMPL
// These functions return the quotient of the signed division of a and b.
int32_t OPTIMIZED __divsi3(int32_t numerator, int32_t denominator)
{
	if (denominator == 0)
	{
#ifdef _LSL_TARGETARCH_ARM
		THROW_DIV_EXCEPT();
#else
#	error "Platform not supported"
#endif
	}
	// Grumat: this failed to pass unit tests
	return (uint32_t)((double)numerator / (double)denominator);
}
#elif _USE_SOFT_I32_DIV == LLVM_ASM_IMPL
// These functions return the quotient of the signed division of a and b.
uint32_t NAKED __divsi3( uint32_t a )
{
	asm volatile(
		" .syntax unified				\r\n"
		" .align 3						\r\n"
		" push    {r4, r7, lr}          \r\n"
		" add     r7, sp, #4            \r\n"
		" eor     r4, r0, r1			\r\n"
		" eor     r2, r0, r0, asr #31	\r\n"
		" eor     r3, r1, r1, asr #31	\r\n"
		" sub     r0, r2, r0, asr #31	\r\n"
		" sub     r1, r3, r1, asr #31	\r\n"
		" bl      __udivsi3				\r\n"
		" eor     r0, r0, r4, asr #31	\r\n"
		" sub     r0, r0, r4, asr #31	\r\n"
		" pop     {r4, r7, pc}			\r\n"
		" .syntax divided				\r\n"
		: : : "cc", "r2", "r3", "r4", "r7", "lr");
	return a;
}
#elif _USE_SOFT_I32_DIV == SIGMATEK_IMPL
static uint32_t OPTIMIZED divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
#define _NEEDS_DIVMODSI4
// These functions return the quotient of the signed division of a and b.
int32_t OPTIMIZED __divsi3 (int32_t numerator, int32_t denominator)
{
#ifdef _LSL_TARGETARCH_ARM
	if (denominator == 0)
	{
		THROW_DIV_EXCEPT();
	}
#endif
	int sign;
	long dividend;
	divnorm (numerator, denominator, sign);

	dividend = divmodsi4 (0,  numerator, denominator);
	exitdiv (sign, dividend);
}
#elif _USE_SOFT_I32_DIV != NO_IMPL
#	error "_USE_SOFT_I32_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_I32_DIV == IFP_IMPL


#if _USE_SOFT_U32_DIV == IFP_IMPL
// These functions return the quotient of the unsigned division of a and b.
uint32_t OPTIMIZED __udivsi3(uint32_t numerator, uint32_t denominator)
{
	if (denominator == 0)
	{
#ifdef _LSL_TARGETARCH_ARM
		THROW_DIV_EXCEPT();
#else
#	error "Platform not supported"
#endif
	}
	return (uint32_t)((double)numerator/(double)denominator);
}
#elif _USE_SOFT_U32_DIV == LLVM_ASM_IMPL
// These functions return the quotient of the unsigned division of a and b.
void NAKED __udivsi3(uint32_t numerator, uint32_t denominator)
{
	asm volatile(
		" .syntax unified				\r\n"
		" push   {r7, lr}				\r\n"
		" mov     r7, sp                \r\n"
		" clz     r2, r0				\r\n"
		" tst     r1, r1				\r\n"
		" clz     r3, r1				\r\n"
		" mov     ip, #0				\r\n"
		" bne     .L_argvalid			\r\n"
		" .word   0xf7f5f5f2			\r\n"
		".L_argvalid:					\r\n"
		" mov     lr, #1				\r\n"
		" subs    r3, r3, r2			\r\n"
		" blt     .L_return				\r\n"
		".L_mainLoop:					\r\n"
		" subs    r2, r0, r1, lsl r3	\r\n"
		" orrhs   ip, ip, lr, lsl r3	\r\n"
		" movhs   r0, r2				\r\n"
		" subsne  r3, r3, #1			\r\n"
		" bhi     .L_mainLoop			\r\n"
		" subs    r2, r0, r1			\r\n"
		" orrhs   ip, #1				\r\n"
		".L_return:						\r\n"
		" mov     r0, ip				\r\n"
		" pop    {r7, pc}               \r\n"
		" .syntax divided				\r\n"
		: : : "cc", "r2", "r3", "r7", "lr");
}
#elif _USE_SOFT_U32_DIV == SIGMATEK_IMPL
static uint32_t OPTIMIZED divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
#define _NEEDS_DIVMODSI4
// These functions return the quotient of the unsigned division of a and b.
uint32_t OPTIMIZED __udivsi3 (uint32_t numerator, uint32_t denominator)
{
	if (denominator == 0)
	{
#ifdef _LSL_TARGETARCH_ARM
		THROW_DIV_EXCEPT();
#else
#	error "Platform not supported"
#endif
	}
	return  divmodsi4(0, numerator, denominator);
}
#elif _USE_SOFT_U32_DIV != NO_IMPL
#	error "_USE_SOFT_U32_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_U32_DIV == IFP_IMPL


#if _USE_SOFT_I32_MOD == SIGMATEK_IMPL
static uint32_t OPTIMIZED divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
#define _NEEDS_DIVMODSI4
int32_t OPTIMIZED __modsi3 (int32_t numerator, int32_t denominator)
{
	if (denominator == 0)
	{
#ifdef _LSL_TARGETARCH_ARM
		THROW_DIV_EXCEPT();
#else
#	error "Platform not supported"
#endif
	}
	int sign = 0;
	int32_t modul;

	if (numerator < 0)
	{
		numerator = -numerator;
		sign = 1;
	}
	if (denominator < 0)
	{
		denominator = -denominator;
	}

	modul =  divmodsi4(1, numerator, denominator);
	if (sign)
		return -modul;
	return modul;
}
#elif _USE_SOFT_I32_MOD != NO_IMPL
#	error "_USE_SOFT_I32_MOD is missing, invalid or unsupported"
#endif	// _USE_SOFT_I32_MOD == SIGMATEK_IMPL


#if _USE_SOFT_U32_MOD == SIGMATEK_IMPL
static uint32_t OPTIMIZED divmodsi4(int32_t modwanted, uint32_t num, uint32_t den);
#define _NEEDS_DIVMODSI4
// These functions return the remainder of the unsigned division of a and b.
uint32_t OPTIMIZED __umodsi3 (uint32_t numerator, uint32_t denominator)
{
	if (denominator == 0)
	{
#ifdef _LSL_TARGETARCH_ARM
		THROW_DIV_EXCEPT();
#else
#	error "Platform not supported"
#endif
	}
	return  divmodsi4 (1, numerator, denominator);
}
#elif _USE_SOFT_U32_MOD != NO_IMPL
#	error "_USE_SOFT_U32_MOD is missing, invalid or unsupported"
#endif	// _USE_SOFT_U32_MOD == SIGMATEK_IMPL


#ifdef _NEEDS_DIVMODSI4
static uint32_t OPTIMIZED divmodsi4(int32_t modwanted, uint32_t num, uint32_t den)
{
	register uint32_t bit = 1;
	register uint32_t res = 0;
	while (den < num && bit && !(den & (1L << 31)))
	{
		den <<= 1;
		bit <<= 1;
	}
	while (bit)
	{
		if (num >= den)
		{
			num -= den;
			res |= bit;
		}
		bit >>= 1;
		den >>= 1;
	}
	if (modwanted)
		return num;
	return res;
}
#endif

// ********************************************************************************************
// 64 BIT Funktionen
// ********************************************************************************************


si_int __ctzsi2(si_int a)
{
	su_int x = (su_int)a;
	si_int t = ((x & 0x0000FFFF) == 0) << 4;  /* if (x has no small bits) t = 16 else 0 */
	x >>= t;           /* x = [0 - 0xFFFF] + higher garbage bits */
	su_int r = t;       /* r = [0, 16]  */
						/* return r + ctz(x) */
	t = ((x & 0x00FF) == 0) << 3;
	x >>= t;           /* x = [0 - 0xFF] + higher garbage bits */
	r += t;            /* r = [0, 8, 16, 24] */
					   /* return r + ctz(x) */
	t = ((x & 0x0F) == 0) << 2;
	x >>= t;           /* x = [0 - 0xF] + higher garbage bits */
	r += t;            /* r = [0, 4, 8, 12, 16, 20, 24, 28] */
					   /* return r + ctz(x) */
	t = ((x & 0x3) == 0) << 1;
	x >>= t;
	x &= 3;            /* x = [0 - 3] */
	r += t;            /* r = [0 - 30] and is even */
					   /* return r + ctz(x) */

	/*  The branch-less return statement below is equivalent
	**  to the following switch statement:
	**     switch (x)
	**    {
	**     case 0:
	**         return r + 2;
	**     case 2:
	**         return r + 1;
	**     case 1:
	**     case 3:
	**         return r;
	**     }
	*/
	return r + ((2 - (x >> 1)) & -((x & 1) == 0));
}


#if _USE_SOFT_I64_DIV == LLVM_C_IMPL
#define _NEEDS_UDIVMODDI4
du_int __udivmoddi4(du_int a, du_int b, du_int* rem);
di_int __divdi3(di_int a, di_int b)
{
	const int bits_in_dword_m1 = (int)(sizeof(di_int) * CHAR_BIT) - 1;
	di_int s_a = a >> bits_in_dword_m1;           /* s_a = a < 0 ? -1 : 0 */
	di_int s_b = b >> bits_in_dword_m1;           /* s_b = b < 0 ? -1 : 0 */
	a = (a ^ s_a) - s_a;                         /* negate if s_a == -1 */
	b = (b ^ s_b) - s_b;                         /* negate if s_b == -1 */
	s_a ^= s_b;                                  /*sign of quotient */
	return (__udivmoddi4(a, b, (du_int*)0) ^ s_a) - s_a;  /* negate if s_a == -1 */
}
#elif _USE_SOFT_I64_DIV != NO_IMPL
#	error "_USE_SOFT_I64_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_I64_DIV == LLVM_C_IMPL


#if _USE_SOFT_U64_DIV == LLVM_C_IMPL
#define _NEEDS_UDIVMODDI4
du_int __udivmoddi4(du_int a, du_int b, du_int* rem);
du_int __udivdi3(du_int a, du_int b)
{
	return __udivmoddi4(a, b, 0);
}
#elif _USE_SOFT_U64_DIV != NO_IMPL
#	error "_USE_SOFT_U64_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_U64_DIV == LLVM_C_IMPL


// DS5303
#if _USE_SOFT_I64_MOD == LLVM_C_IMPL
#define _NEEDS_UDIVMODDI4
du_int __udivmoddi4(du_int a, du_int b, du_int* rem);
di_int __moddi3(di_int a, di_int b)
{
	const int bits_in_dword_m1 = (int)(sizeof(di_int) * CHAR_BIT) - 1;
	di_int s = b >> bits_in_dword_m1;  /* s = b < 0 ? -1 : 0 */
	b = (b ^ s) - s;                   /* negate if s == -1 */
	s = a >> bits_in_dword_m1;         /* s = a < 0 ? -1 : 0 */
	a = (a ^ s) - s;                   /* negate if s == -1 */
	di_int r;
	__udivmoddi4(a, b, (du_int*)&r);
	return (r ^ s) - s;                /* negate if s == -1 */
}
#elif _USE_SOFT_I64_MOD != NO_IMPL
#	error "_USE_SOFT_I64_MOD is missing, invalid or unsupported"
#endif	// _USE_SOFT_I64_MOD == LLVM_C_IMPL


#if _USE_SOFT_U64_MOD == LLVM_C_IMPL
#define _NEEDS_UDIVMODDI4
du_int __udivmoddi4(du_int a, du_int b, du_int* rem);
du_int __umoddi3(du_int a, du_int b)
{
	du_int r;
	__udivmoddi4(a, b, &r);
	return r;
}
#elif _USE_SOFT_U64_MOD != NO_IMPL
#	error "_USE_SOFT_U64_MOD is missing, invalid or unsupported"
#endif	// _USE_SOFT_U64_MOD == LLVM_C_IMPL


#ifdef _NEEDS_UDIVMODDI4
du_int __udivmoddi4(du_int a, du_int b, du_int* rem)
{
	const unsigned n_uword_bits = sizeof(su_int) * CHAR_BIT;
	const unsigned n_udword_bits = sizeof(du_int) * CHAR_BIT;
	udwords n;
	n.all = a;
	udwords d;
	d.all = b;
	udwords q;
	udwords r;
	unsigned sr;
	/* special cases, X is unknown, K != 0 */
	if (n.s.high == 0)
	{
		if (d.s.high == 0)
		{
			/* 0 X
			** ---
			** 0 X
			*/
#ifdef _LSL_TARGETARCH_ARM
			if (d.s.low == 0)
				THROW_DIV_EXCEPT();
#endif
			if (rem)
				*rem = n.s.low % d.s.low;
			return n.s.low / d.s.low;
		}
		/* 0 X
		** ---
		** K X
		*/
		if (rem)
			*rem = n.s.low;
		return 0;
	}
	/* n.s.high != 0 */
	if (d.s.low == 0)
	{
		if (d.s.high == 0)
		{
			/* K X
			** ---
			** 0 0
			*/
#ifdef _LSL_TARGETARCH_ARM
			THROW_DIV_EXCEPT();
#endif
			if (rem)
				*rem = n.s.high % d.s.low;
			return n.s.high / d.s.low;
		}
		/* d.s.high != 0 */
		if (n.s.low == 0)
		{
			/* K 0
			** ---
			** K 0
			*/
			if (rem)
			{
				r.s.high = n.s.high % d.s.high;
				r.s.low = 0;
				*rem = r.all;
			}
			return n.s.high / d.s.high;
		}
		/* K K
		** ---
		** K 0
		*/
		if ((d.s.high & (d.s.high - 1)) == 0)     /* if d is a power of 2 */
		{
			if (rem)
			{
				r.s.low = n.s.low;
				r.s.high = n.s.high & (d.s.high - 1);
				*rem = r.all;
			}
			return n.s.high >> __builtin_ctz(d.s.high);
		}
		/* K K
		** ---
		** K 0
		*/
		sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
		/* 0 <= sr <= n_uword_bits - 2 or sr large */
		if (sr > n_uword_bits - 2)
		{
			if (rem)
				*rem = n.all;
			return 0;
		}
		++sr;
		/* 1 <= sr <= n_uword_bits - 1 */
		/* q.all = n.all << (n_udword_bits - sr); */
		q.s.low = 0;
		q.s.high = n.s.low << (n_uword_bits - sr);
		/* r.all = n.all >> sr; */
		r.s.high = n.s.high >> sr;
		r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
	}
	else  /* d.s.low != 0 */
	{
		if (d.s.high == 0)
		{
			/* K X
			** ---
			** 0 K
			*/
			if ((d.s.low & (d.s.low - 1)) == 0)     /* if d is a power of 2 */
			{
				if (rem)
					*rem = n.s.low & (d.s.low - 1);
				if (d.s.low == 1)
					return n.all;
				sr = __builtin_ctz(d.s.low);
				q.s.high = n.s.high >> sr;
				q.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
				return q.all;
			}
			/* K X
			** ---
			** 0 K
			*/
			sr = 1 + n_uword_bits + __builtin_clz(d.s.low) - __builtin_clz(n.s.high);
			/* 2 <= sr <= n_udword_bits - 1
			** q.all = n.all << (n_udword_bits - sr);
			** r.all = n.all >> sr;
			** if (sr == n_uword_bits)
			** {
			**     q.s.low = 0;
			**     q.s.high = n.s.low;
			**     r.s.high = 0;
			**     r.s.low = n.s.high;
			** }
			** else if (sr < n_uword_bits)  // 2 <= sr <= n_uword_bits - 1
			** {
			**     q.s.low = 0;
			**     q.s.high = n.s.low << (n_uword_bits - sr);
			**     r.s.high = n.s.high >> sr;
			**     r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
			** }
			** else              // n_uword_bits + 1 <= sr <= n_udword_bits - 1
			** {
			**     q.s.low = n.s.low << (n_udword_bits - sr);
			**     q.s.high = (n.s.high << (n_udword_bits - sr)) |
			**              (n.s.low >> (sr - n_uword_bits));
			**     r.s.high = 0;
			**     r.s.low = n.s.high >> (sr - n_uword_bits);
			** }
			**/
			q.s.low = (n.s.low << (n_udword_bits - sr)) &
				((si_int)(n_uword_bits - sr) >> (n_uword_bits - 1));
			q.s.high = ((n.s.low << (n_uword_bits - sr))                       &
				((si_int)(sr - n_uword_bits - 1) >> (n_uword_bits - 1))) |
				(((n.s.high << (n_udword_bits - sr)) |
				(n.s.low >> (sr - n_uword_bits)))                        &
					((si_int)(n_uword_bits - sr) >> (n_uword_bits - 1)));
			r.s.high = (n.s.high >> sr) &
				((si_int)(sr - n_uword_bits) >> (n_uword_bits - 1));
			r.s.low = ((n.s.high >> (sr - n_uword_bits))                       &
				((si_int)(n_uword_bits - sr - 1) >> (n_uword_bits - 1))) |
				(((n.s.high << (n_uword_bits - sr)) |
				(n.s.low >> sr))                                         &
					((si_int)(sr - n_uword_bits) >> (n_uword_bits - 1)));
		}
		else
		{
			/* K X
			** ---
			** K K
			*/
			sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
			/* 0 <= sr <= n_uword_bits - 1 or sr large */
			if (sr > n_uword_bits - 1)
			{
				if (rem)
					*rem = n.all;
				return 0;
			}
			++sr;
			/* 1 <= sr <= n_uword_bits */
			/*  q.all = n.all << (n_udword_bits - sr); */
			q.s.low = 0;
			q.s.high = n.s.low << (n_uword_bits - sr);
			/* r.all = n.all >> sr;
			** if (sr < n_uword_bits)
			** {
			**     r.s.high = n.s.high >> sr;
			**     r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
			** }
			** else
			** {
			**     r.s.high = 0;
			**     r.s.low = n.s.high;
			** }
			*/
			r.s.high = (n.s.high >> sr) &
				((si_int)(sr - n_uword_bits) >> (n_uword_bits - 1));
			r.s.low = (n.s.high << (n_uword_bits - sr)) |
				((n.s.low >> sr)                  &
				((si_int)(sr - n_uword_bits) >> (n_uword_bits - 1)));
		}
	}
	/* Not a special case
	** q and r are initialized with:
	** q.all = n.all << (n_udword_bits - sr);
	** r.all = n.all >> sr;
	** 1 <= sr <= n_udword_bits - 1
	*/
	su_int carry = 0;
	for (; sr > 0; --sr)
	{
		/* r:q = ((r:q)  << 1) | carry */
		r.s.high = (r.s.high << 1) | (r.s.low >> (n_uword_bits - 1));
		r.s.low = (r.s.low << 1) | (q.s.high >> (n_uword_bits - 1));
		q.s.high = (q.s.high << 1) | (q.s.low >> (n_uword_bits - 1));
		q.s.low = (q.s.low << 1) | carry;
		/* carry = 0;
		** if (r.all >= d.all)
		** {
		**      r.all -= d.all;
		**      carry = 1;
		** }
		*/
		const di_int s = (di_int)(d.all - r.all - 1) >> (n_udword_bits - 1);
		carry = s & 1;
		r.all -= d.all & s;
	}
	q.all = (q.all << 1) | carry;
	if (rem)
		*rem = r.all;
	return q.all;
}
#endif	// _NEEDS_UDIVMODDI4


#if _USE_SOFT_I64_CMP == SIGMATEK_IMPL
int32_t OPTIMIZED __cmpdi2(int64_t a, int64_t b)
{
  DWunion au;
  DWunion bu;
  au.ll = a;
  bu.ll = b;

  if (au.s.high < bu.s.high)
	return 0;
  else if (au.s.high > bu.s.high)
	return 2;
  if ((UWtype) au.s.low < (UWtype) bu.s.low)
	return 0;
  else if ((UWtype) au.s.low > (UWtype) bu.s.low)
	return 2;
  return 1; 
}
#elif _USE_SOFT_I64_CMP != NO_IMPL
#	error "_USE_SOFT_I64_CMP is missing, invalid or unsupported"
#endif	// _USE_SOFT_I64_CMP == SIGMATEK_IMPL


#if _USE_SOFT_U64_CMP == SIGMATEK_IMPL
int32_t OPTIMIZED __ucmpdi2(uint64_t a, uint64_t b)
{
	DWunion au;
	DWunion bu;
	au.ll = a;
	bu.ll = b;

	if ((uint32_t)au.s.high < (uint32_t)bu.s.high)
		return 0;
	else if ((uint32_t)au.s.high > (uint32_t)bu.s.high)
		return 2;
	if ((uint32_t)au.s.low < (uint32_t)bu.s.low)
		return 0;
	else if ((uint32_t)au.s.low > (uint32_t)bu.s.low)
		return 2;
	return 1;
}
#elif _USE_SOFT_U64_CMP != NO_IMPL
#	error "_USE_SOFT_U64_CMP is missing, invalid or unsupported"
#endif	// _USE_SOFT_U64_CMP == SIGMATEK_IMPL


// ********************************************************************************************
// Diverse Funktionen
// ********************************************************************************************

uint32_t OPTIMIZED __clzsi2(uint32_t a)
{
#if _USE_SOFT_U32_CLZ == LLVM_ASM_IMPL
	uint32_t r;
	CLZ_32(r,a);
	return r;
#elif _USE_SOFT_U32_CLZ == LLVM_C_IMPL
	su_int x = (su_int)a;
	si_int t = ((x & 0xFFFF0000) == 0) << 4; /* if (x is small) t = 16 else 0 */
	x >>= 16 - t;		/* x = [0 - 0xFFFF] */
	su_int r = t;		/* r = [0, 16] */
	/* return r + clz(x) */
	t = ((x & 0xFF00) == 0) << 3;
	x >>= 8 - t;		/* x = [0 - 0xFF] */
	r += t;				/* r = [0, 8, 16, 24] */
	/* return r + clz(x) */
	t = ((x & 0xF0) == 0) << 2;
	x >>= 4 - t;		/* x = [0 - 0xF] */
	r += t;				/* r = [0, 4, 8, 12, 16, 20, 24, 28] */
	/* return r + clz(x) */
	t = ((x & 0xC) == 0) << 1;
	x >>= 2 - t;		/* x = [0 - 3] */
	r += t;				/* r = [0 - 30] and is even */
	/* return r + clz(x) */
/*     switch (x)
**     {
**     case 0:
**         return r + 2;
**     case 1:
**         return r + 1;
**     case 2:
**     case 3:
**         return r;
**     }
*/
	return r + ((2 - x) & -((x & 2) == 0));
#elif _USE_SOFT_U32_CLZ != NO_IMPL
#	error "_USE_SOFT_U32_CLZ is missing, invalid or unsupported"
#endif
}

uint32_t OPTIMIZED __clzdi2(uint64_t v)
{
#if _USE_SOFT_U64_CLZ == LLVM_ASM_IMPL
	uint32_t r;
	REGISTER uint32_t vlo = (uint32_t)v;
	REGISTER uint32_t vhi = (uint32_t)(v >> 32);
	CLZ_64(r, vhi, vlo);
	return r;
#elif _USE_SOFT_U64_CLZ == LLVM_C_IMPL
	dwords x;
	x.all = v;
	const si_int f = -(x.s.high == 0);
	return __builtin_clz((x.s.high & ~f) | (x.s.low & f)) +
		(f & ((si_int)(sizeof(si_int) * CHAR_BIT)));
#elif _USE_SOFT_U64_CLZ != NO_IMPL
#	error "_USE_SOFT_U64_CLZ is missing, invalid or unsupported"
#endif
}

// ********************************************************************************************
// SOFT Float Funktionen
// ********************************************************************************************


#ifdef _USE_STK_SOFT_FLOAT

#define FLOAT32_MAX 0x7f800000
#define FLOAT32_MIN 0xff800000
#define FLOAT64_MAX
#define FLOAT64_MIN

/*
 * For recognizing NaNs or infinity use isFloat32NaN and is Float32Inf,
 * comparing with these constants is not sufficient.
 */
#define FLOAT32_NAN             0x7FC00001
#define FLOAT32_SIGNAN          0x7F800001
#define FLOAT32_INF             0x7F800000

#define FLOAT64_NAN             0x7FF8000000000001ll
#define FLOAT64_SIGNAN          0x7FF0000000000001ll
#define FLOAT64_INF             0x7FF0000000000000ll

#define FLOAT32_FRACTION_SIZE   23
#define FLOAT64_FRACTION_SIZE   52

#define FLOAT32_HIDDEN_BIT_MASK 0x800000
#define FLOAT64_HIDDEN_BIT_MASK 0x10000000000000ll

#define FLOAT32_MAX_EXPONENT    0xFF
#define FLOAT64_MAX_EXPONENT    0x7FF

#define FLOAT32_BIAS            0x7F
#define FLOAT64_BIAS            0x3FF
#define FLOAT80_BIAS            0x3FFF
 
typedef union {
	float f;
	uint32_t binary;

	struct 	{
#if defined(ARCH_IS_BIG_ENDIAN)
		uint32_t sign:1;
		uint32_t exp:8;
		uint32_t fraction:23;
#elif defined(ARCH_IS_LITTLE_ENDIAN)
		uint32_t fraction:23;
		uint32_t exp:8;
		uint32_t sign:1;
#else 
#error "Unknown endians."
#endif
		} parts __attribute__ ((packed));
	} float32;
	
typedef union {
	double d;
	uint64_t binary;
	
	struct	{
#if defined(ARCH_IS_BIG_ENDIAN)
		uint64_t sign:1;
		uint64_t exp:11;
		uint64_t fraction:52;
#elif defined(ARCH_IS_LITTLE_ENDIAN)
		uint64_t fraction:52;
		uint64_t exp:11;
		uint64_t sign:1;
#else 
#error "Unknown endians."
#endif
		} parts __attribute__ ((packed));
	} float64;


/////////////// Function inclusion logic ///////////////
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT32_CMP
#endif
#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT64_CMP
#endif
#if _USE_SOFT_F32_F64 == SIGMATEK_IMPL
#	define _NEEDS_CONVERTFLOAT32TOFLOAT64
#	define _NEEDS_FLOAT32_TEST
#endif
#if _USE_SOFT_F64_F32 == SIGMATEK_IMPL
#	define _NEEDS_CONVERTFLOAT64TOFLOAT32
#	define _NEEDS_FLOAT64_TEST
#endif
#if _USE_SOFT_F32_U32 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT32_TO_UINT32
#	define _NEEDS_FLOAT32_TO_UINT32_HELPER
#	define _NEEDS_FLOAT32_TEST
#endif
#if _USE_SOFT_F32_I32 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT32_TO_INT32
#	define _NEEDS_FLOAT32_TO_UINT32_HELPER
#	define _NEEDS_FLOAT32_TEST
#endif
#if (_USE_SOFT_F64_U64 == SIGMATEK_IMPL) || (_USE_SOFT_F64_U128 == SIGMATEK_IMPL)
#	define _NEEDS_FLOAT64_TO_UINT64
#	define _NEEDS_FLOAT64_TO_UINT64_HELPER
#	define _NEEDS_FLOAT64_TEST
#endif
#if (_USE_SOFT_F64_I64 == SIGMATEK_IMPL) || (_USE_SOFT_F64_I128 == SIGMATEK_IMPL)
#	define _NEEDS_FLOAT64_TO_INT64
#	define _NEEDS_FLOAT64_TO_UINT64_HELPER
#	define _NEEDS_FLOAT64_TEST
#endif
#if (_USE_SOFT_F32_U64 == SIGMATEK_IMPL) || (_USE_SOFT_F32_U128 == SIGMATEK_IMPL)
#	define _NEEDS_FLOAT32_TO_UINT64
#	define _NEEDS_FLOAT32_TO_UINT64_HELPER
#	define _NEEDS_FLOAT32_TEST
#endif
#if (_USE_SOFT_F32_I64 == SIGMATEK_IMPL) || (_USE_SOFT_F32_I128 == SIGMATEK_IMPL)
#	define _NEEDS_FLOAT32_TO_INT64
#	define _NEEDS_FLOAT32_TO_UINT64_HELPER
#	define _NEEDS_FLOAT32_TEST
#endif
#if _USE_SOFT_F64_U32 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT64_TO_UINT32
#	define _NEEDS_FLOAT64_TO_UINT64_HELPER
#	define _NEEDS_FLOAT64_TEST
#endif
#if _USE_SOFT_F64_I32 == SIGMATEK_IMPL
#	define _NEEDS_FLOAT64_TO_INT32
#	define _NEEDS_FLOAT64_TO_UINT64_HELPER
#	define _NEEDS_FLOAT64_TEST
#endif
#if _USE_SOFT_U32_F32 == SIGMATEK_IMPL
#	define _NEEDS_UINT32_TO_FLOAT32
#	define _NEEDS_ROUNDFLOAT32
#endif
#if _USE_SOFT_I32_F32 == SIGMATEK_IMPL
#	define _NEEDS_INT32_TO_FLOAT32
#	define _NEEDS_UINT32_TO_FLOAT32
#	define _NEEDS_ROUNDFLOAT32
#endif
#if (_USE_SOFT_U64_F32 == SIGMATEK_IMPL) || (_USE_SOFT_U128_F32 == SIGMATEK_IMPL)
#	define _NEEDS_UINT64_TO_FLOAT32
#	define _NEEDS_ROUNDFLOAT32
#endif
#if (_USE_SOFT_I64_F32 == SIGMATEK_IMPL) || (_USE_SOFT_I128_F32 == SIGMATEK_IMPL)
#	define _NEEDS_INT64_TO_FLOAT32
#	define _NEEDS_UINT64_TO_FLOAT32
#	define _NEEDS_ROUNDFLOAT32
#endif
#if (_USE_SOFT_U32_F64 == SIGMATEK_IMPL)
#	define _NEEDS_UINT32_TO_FLOAT64
#	define _NEEDS_ROUNDFLOAT64
#endif
#if (_USE_SOFT_I32_F64 == SIGMATEK_IMPL)
#	define _NEEDS_INT32_TO_FLOAT64
#	define _NEEDS_UINT32_TO_FLOAT64
#	define _NEEDS_ROUNDFLOAT64
#endif
#if (_USE_SOFT_U64_F64 == SIGMATEK_IMPL) || (_USE_SOFT_U128_F64 == SIGMATEK_IMPL)
#	define _NEEDS_UINT64_TO_FLOAT64
#	define _NEEDS_ROUNDFLOAT64
#endif
#if (_USE_SOFT_I64_F64 == SIGMATEK_IMPL) || (_USE_SOFT_I128_F64 == SIGMATEK_IMPL)
#	define _NEEDS_INT64_TO_FLOAT64
#	define _NEEDS_UINT64_TO_FLOAT64
#	define _NEEDS_ROUNDFLOAT64
#endif
#if (_USE_SOFT_F32_ADD == SIGMATEK_IMPL) || (_USE_SOFT_F32_SUB == SIGMATEK_IMPL)
#	define _NEEDS_ADDFLOAT32
#	define _NEEDS_SUBFLOAT32
#	define _NEEDS_FLOAT32_TEST
#endif
#if (_USE_SOFT_F64_ADD == SIGMATEK_IMPL) || (_USE_SOFT_F64_SUB == SIGMATEK_IMPL)
#	define _NEEDS_ADDFLOAT64
#	define _NEEDS_SUBFLOAT64
#	define _NEEDS_FLOAT64_TEST
#endif
#if (_USE_SOFT_F32_MUL == SIGMATEK_IMPL)
#	define _NEEDS_MULFLOAT32
#	define _NEEDS_FLOAT32_TEST
#endif
#if (_USE_SOFT_F64_MUL == SIGMATEK_IMPL)
#	define _NEEDS_MULFLOAT64
#	define _NEEDS_MUL64INTEGERS
#	define _NEEDS_FLOAT64_TEST
#endif
#if (_USE_SOFT_F32_DIV == SIGMATEK_IMPL)
#	define _NEEDS_DIVFLOAT32
#	define _NEEDS_FLOAT32_TEST
#endif
#if (_USE_SOFT_F64_DIV == SIGMATEK_IMPL)
#	define _NEEDS_DIVFLOAT64
#	define _NEEDS_MUL64INTEGERS
#	define _NEEDS_FLOAT64_TEST
#endif



uint64_t divFloat64estim(uint64_t a, uint64_t b);


#ifdef _NEEDS_FLOAT32_TEST
inline int OPTIMIZED isFloat32NaN(float32 f)
{	/* NaN : exp = 0xff and nonzero fraction */
	return ((f.parts.exp == 0xFF) && (f.parts.fraction));
}

inline int OPTIMIZED isFloat32SigNaN(float32 f)
{	/* SigNaN : exp = 0xff fraction = 0xxxxx..x (binary), where at least one x is nonzero */
	return ((f.parts.exp == 0xFF) && (f.parts.fraction<0x400000) && (f.parts.fraction));
}

inline int OPTIMIZED isFloat32Infinity(float32 f)
{
	return ((f.parts.exp == 0xFF) && (f.parts.fraction == 0x0));
}

inline int OPTIMIZED isFloat32Zero(float32 f)
{
	return (((f.binary) & 0x7FFFFFFF) == 0);
}
#endif	// _NEEDS_FLOAT32_TEST


#ifdef _NEEDS_FLOAT64_TEST
inline int OPTIMIZED isFloat64NaN(float64 d)
{	/* NaN : exp = 0x7ff and nonzero fraction */
	return ((d.parts.exp == 0x7FF) && (d.parts.fraction));
}

inline int OPTIMIZED isFloat64SigNaN(float64 d)
{	/* SigNaN : exp = 0x7ff fraction = 0xxxxx..x (binary), where at least one x is nonzero */
	return ((d.parts.exp == 0x7FF) && (d.parts.fraction) && (d.parts.fraction<0x8000000000000ll));
}

inline int OPTIMIZED isFloat64Infinity(float64 d)
{
	return ((d.parts.exp == 0x7FF) && (d.parts.fraction == 0x0));
}

inline int OPTIMIZED isFloat64Zero(float64 d)
{
	return (((d.binary) & 0x7FFFFFFFFFFFFFFFll) == 0);
}
#endif	// _NEEDS_FLOAT64_TEST


#ifdef _NEEDS_FLOAT32_CMP
/**
 * @return 1, if both floats are equal - but NaNs are not recognized 
 */
inline int OPTIMIZED isFloat32eq(float32 a, float32 b)
{
	return ((a.binary==b.binary)||(((a.binary| b.binary)&0x7FFFFFFF)==0)); /* a equals to b or both are zeros (with any sign) */
}


/**
 * @return 1, if a<b - but NaNs are not recognized 
 */
inline int OPTIMIZED isFloat32lt(float32 a, float32 b)
{
	if (((a.binary| b.binary)&0x7FFFFFFF)==0) {
		return 0; /* +- zeroes */
	};
	
	if ((a.parts.sign)&&(b.parts.sign)) {
		/*if both are negative, smaller is that with greater binary value*/
		return (a.binary>b.binary);
		};
	
	/* lets negate signs - now will be positive numbers allways bigger than negative (first bit will be set for unsigned integer comparison)*/
	a.parts.sign=!a.parts.sign;
	b.parts.sign=!b.parts.sign;
	return (a.binary<b.binary);
			
}

/**
 * @return 1, if a>b - but NaNs are not recognized 
 */
inline int OPTIMIZED isFloat32gt(float32 a, float32 b)
{
	if (((a.binary| b.binary)&0x7FFFFFFF)==0) {
		return 0; /* zeroes are equal with any sign */
	};
	
	if ((a.parts.sign)&&(b.parts.sign)) {
		/*if both are negative, greater is that with smaller binary value*/
		return (a.binary<b.binary);
		};
	
	/* lets negate signs - now will be positive numbers allways bigger than negative (first bit will be set for unsigned integer comparison)*/
	a.parts.sign=!a.parts.sign;
	b.parts.sign=!b.parts.sign;
	return (a.binary>b.binary);
			
}
#endif	// _NEEDS_FLOAT32_CMP


#ifdef _NEEDS_FLOAT64_CMP
/**
 * @return 1, if both floats are equal - but NaNs are not recognized 
 */
inline int OPTIMIZED isFloat64eq(float64 a, float64 b)
{
	return ((a.binary==b.binary)||(((a.binary| b.binary)&0x7FFFFFFFFFFFFFFFLL)==0)); /* a equals to b or both are zeros (with any sign) */
}


/**
 * @return 1, if a<b - but NaNs are not recognized 
 */
inline int OPTIMIZED isFloat64lt(float64 a, float64 b)
{
  if (((a.binary| b.binary)&0x7FFFFFFFFFFFFFFFLL)==0) {
		return 0; /* +- zeroes */
	};
	
	if ((a.parts.sign)&&(b.parts.sign)) {
		/*if both are negative, smaller is that with greater binary value*/
		return (a.binary>b.binary);
		};
	
	/* lets negate signs - now will be positive numbers allways bigger than negative (first bit will be set for unsigned integer comparison)*/
	a.parts.sign=!a.parts.sign;
	b.parts.sign=!b.parts.sign;
	return (a.binary<b.binary);
			
}


inline int OPTIMIZED isFloat64gt(float64 a, float64 b)
{
	if (((a.binary| b.binary)&0x7FFFFFFFFFFFFFFFLL)==0) {
		return 0; /* zeroes are equal with any sign */
	};
	
	if ((a.parts.sign)&&(b.parts.sign)) {
		/*if both are negative, greater is that with smaller binary value*/
		return (a.binary<b.binary);
		};
	
	/* lets negate signs - now will be positive numbers allways bigger than negative (first bit will be set for unsigned integer comparison)*/
	a.parts.sign=!a.parts.sign;
	b.parts.sign=!b.parts.sign;
	return (a.binary>b.binary);
			
}
#endif	// _NEEDS_FLOAT64_CMP


#ifdef _NEEDS_CONVERTFLOAT32TOFLOAT64
float64 OPTIMIZED convertFloat32ToFloat64(float32 a)
{
	float64 result;
	uint64_t frac;
	
	result.parts.sign = a.parts.sign;
	result.parts.fraction = a.parts.fraction;
	result.parts.fraction <<= (FLOAT64_FRACTION_SIZE - FLOAT32_FRACTION_SIZE );
	
	if ((isFloat32Infinity(a))||(isFloat32NaN(a))) {
		result.parts.exp = 0x7FF;
		/* TODO; check if its correct for SigNaNs*/
		return result;
	};
	
	result.parts.exp = a.parts.exp + ( (int)FLOAT64_BIAS - FLOAT32_BIAS );
	if (a.parts.exp == 0) {
		/* normalize denormalized numbers */

		if (result.parts.fraction == 0ll) { /* fix zero */
			result.parts.exp = 0ll;
			return result;
		}
			
		frac = result.parts.fraction;
		
		while (!(frac & (0x10000000000000ll))) {
			frac <<= 1;
			--result.parts.exp;
		};
		
		++result.parts.exp;
		result.parts.fraction = frac;
	};
	
	return result;
} 
#endif	// _NEEDS_CONVERTFLOAT32TOFLOAT64


#ifdef _NEEDS_CONVERTFLOAT64TOFLOAT32
float32 OPTIMIZED convertFloat64ToFloat32(float64 a)
{
	float32 result;
	int32_t exp;
	uint64_t frac;
	
	result.parts.sign = a.parts.sign;
	
	if (isFloat64NaN(a)) {
		
		result.parts.exp = 0xFF;
		
		if (isFloat64SigNaN(a)) {
			result.parts.fraction = 0x400000; /* set first bit of fraction nonzero */
			return result;
		}
	
		result.parts.fraction = 0x1; /* fraction nonzero but its first bit is zero */
		return result;
	};

	if (isFloat64Infinity(a)) {
		result.parts.fraction = 0;
		result.parts.exp = 0xFF;
		return result;
	};

	exp = (int)a.parts.exp - FLOAT64_BIAS + FLOAT32_BIAS;
	
	if (exp >= 0xFF) {
		/*FIXME: overflow*/
		result.parts.fraction = 0;
		result.parts.exp = 0xFF;
		return result;
		
	} else if (exp <= 0 ) {
		
		// underflow or denormalized 		
		result.parts.exp = 0;
		
		exp *= -1;	
		if (exp > FLOAT32_FRACTION_SIZE ) {
			/* FIXME: underflow */
			result.parts.fraction = 0;
			return result;
		};
		
		/* denormalized */
		
		frac = a.parts.fraction; 
		frac |= 0x10000000000000ll; /* denormalize and set hidden bit */
		
		frac >>= (FLOAT64_FRACTION_SIZE - FLOAT32_FRACTION_SIZE + 1);
		
		while (exp > 0) {
			--exp;
			frac >>= 1;
		};
		result.parts.fraction = frac;
		
		return result;
	};

	result.parts.exp = exp;
	result.parts.fraction = a.parts.fraction >> (FLOAT64_FRACTION_SIZE - FLOAT32_FRACTION_SIZE);
	return result;
}
#endif	// _NEEDS_CONVERTFLOAT64TOFLOAT32


#ifdef _NEEDS_ROUNDFLOAT32
/** Round and normalize number expressed by exponent and fraction with first bit (equal to hidden bit) at 30. bit
 * @param exp exponent 
 * @param fraction part with hidden bit shifted to 30. bit
 */
void OPTIMIZED roundFloat32(int32_t *exp, uint32_t *fraction)
{
	/* rounding - if first bit after fraction is set then round up */
	(*fraction) += (0x1 << 6);
	
	if ((*fraction) & (FLOAT32_HIDDEN_BIT_MASK << 8)) { 
		/* rounding overflow */
		++(*exp);
		(*fraction) >>= 1;
	};
	
	if (((*exp) >= FLOAT32_MAX_EXPONENT ) || ((*exp) < 0)) {
		/* overflow - set infinity as result */
		(*exp) = FLOAT32_MAX_EXPONENT;
		(*fraction) = 0;
		return;
	}

	return;
}
#endif	// _NEEDS_ROUNDFLOAT32


#ifdef _NEEDS_ROUNDFLOAT64
/** Round and normalize number expressed by exponent and fraction with first bit (equal to hidden bit) at 62. bit
 * @param exp exponent 
 * @param fraction part with hidden bit shifted to 62. bit
 */
void OPTIMIZED roundFloat64(int32_t *exp, uint64_t *fraction)
{
	/* rounding - if first bit after fraction is set then round up */
	(*fraction) += (0x1 << 9);
	
	if ((*fraction) & (FLOAT64_HIDDEN_BIT_MASK << 11)) { 
		/* rounding overflow */
		++(*exp);
		(*fraction) >>= 1;
	};
	
	if (((*exp) >= FLOAT64_MAX_EXPONENT ) || ((*exp) < 0)) {
		/* overflow - set infinity as result */
		(*exp) = FLOAT64_MAX_EXPONENT;
		(*fraction) = 0;
		return;
	}

	return;
}
#endif	// _NEEDS_ROUNDFLOAT64


#ifdef _NEEDS_FLOAT32_TO_UINT32_HELPER
/** Helping procedure for converting float32 to uint32
 * @param a floating point number in normalized form (no NaNs or Inf are checked )
 * @return unsigned integer
 */
static uint32_t OPTIMIZED _float32_to_uint32_helper(float32 a)
{
	uint32_t frac;
	
	if (a.parts.exp < FLOAT32_BIAS) {
		/*TODO: rounding*/
		return 0;
	}
	
	frac = a.parts.fraction;
	
	frac |= FLOAT32_HIDDEN_BIT_MASK;
	/* shift fraction to left so hidden bit will be the most significant bit */
	frac <<= 32 - FLOAT32_FRACTION_SIZE - 1; 

	frac >>= 32 - (a.parts.exp - FLOAT32_BIAS) - 1;
	if ((a.parts.sign == 1) && (frac != 0)) {
		frac = ~frac;
		++frac;
	}
	
	return frac;
}
#endif	// _NEEDS_FLOAT32_TO_UINT32_HELPER


#ifdef _NEEDS_FLOAT32_TO_UINT32
/* Convert float to unsigned int32
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
uint32_t OPTIMIZED float32_to_uint32(float32 a)
{
	if (isFloat32NaN(a)) {
		return UINT32_MAX;
	}
	
	if (isFloat32Infinity(a) || (a.parts.exp >= (32 + FLOAT32_BIAS)))  {
		if (a.parts.sign) {
			return UINT32_MIN;
		}
		return UINT32_MAX;
	}
	
	return _float32_to_uint32_helper(a);	
}
#endif	// _NEEDS_FLOAT32_TO_UINT32


#ifdef _NEEDS_FLOAT32_TO_INT32
/* Convert float to signed int32
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
int32_t OPTIMIZED float32_to_int32(float32 a)
{
	if (isFloat32NaN(a)) {
		return INT32_MAX;
	}
	
	if (isFloat32Infinity(a) || (a.parts.exp >= (32 + FLOAT32_BIAS)))  {
		if (a.parts.sign) {
			return INT32_MIN;
		}
		return INT32_MAX;
	}
	return _float32_to_uint32_helper(a);
}	
#endif	// _NEEDS_FLOAT32_TO_INT32


#ifdef _NEEDS_FLOAT64_TO_UINT64_HELPER
/** Helping procedure for converting float64 to uint64
 * @param a floating point number in normalized form (no NaNs or Inf are checked )
 * @return unsigned integer
 */
static uint64_t OPTIMIZED _float64_to_uint64_helper(float64 a)
{
	uint64_t frac;
	
	if (a.parts.exp < FLOAT64_BIAS) {
		/*TODO: rounding*/
		return 0;
	}
	
	frac = a.parts.fraction;
	
	frac |= FLOAT64_HIDDEN_BIT_MASK;
	/* shift fraction to left so hidden bit will be the most significant bit */
	frac <<= 64 - FLOAT64_FRACTION_SIZE - 1; 

	frac >>= 64 - (a.parts.exp - FLOAT64_BIAS) - 1;
	if ((a.parts.sign == 1) && (frac != 0)) {
		frac = ~frac;
		++frac;
	}
	
	return frac;
}
#endif	// _NEEDS_FLOAT64_TO_UINT64_HELPER


#ifdef _NEEDS_FLOAT64_TO_UINT64
/* Convert float to unsigned int64
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
uint64_t OPTIMIZED float64_to_uint64(float64 a)
{
	if (isFloat64NaN(a)) {
		return UINT64_MAX;
		}

	if (isFloat64Infinity(a) || (a.parts.exp >= (64 + FLOAT64_BIAS)))  {
		if (a.parts.sign) {
			return UINT64_MIN;
			}
		return UINT64_MAX;
		}

	return _float64_to_uint64_helper(a);
}
#endif	// _NEEDS_FLOAT64_TO_UINT64


#ifdef _NEEDS_FLOAT64_TO_INT64
/* Convert float to signed int64
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
int64_t OPTIMIZED float64_to_int64(float64 a)
{
	if (isFloat64NaN(a)) {
		return INT64_MAX;
		}

	if (isFloat64Infinity(a) || (a.parts.exp >= (64 + FLOAT64_BIAS)))  {
		if (a.parts.sign) {
			return INT64_MIN;
			}
		return INT64_MAX;
	}
	return _float64_to_uint64_helper(a);
}	
#endif	// _NEEDS_FLOAT64_TO_INT64


#ifdef _NEEDS_FLOAT32_TO_UINT64_HELPER
/** Helping procedure for converting float32 to uint64
 * @param a floating point number in normalized form (no NaNs or Inf are checked )
 * @return unsigned integer
 */
static uint64_t OPTIMIZED _float32_to_uint64_helper(float32 a)
{
	uint64_t frac;
	
	if (a.parts.exp < FLOAT32_BIAS) {
		/*TODO: rounding*/
		return 0;
	}
	
	frac = a.parts.fraction;
	
	frac |= FLOAT32_HIDDEN_BIT_MASK;
	/* shift fraction to left so hidden bit will be the most significant bit */
	frac <<= 64 - FLOAT32_FRACTION_SIZE - 1; 

	frac >>= 64 - (a.parts.exp - FLOAT32_BIAS) - 1;
	if ((a.parts.sign == 1) && (frac != 0)) {
		frac = ~frac;
		++frac;
	}
	
	return frac;
}
#endif	// _NEEDS_FLOAT32_TO_UINT64_HELPER


#ifdef _NEEDS_FLOAT32_TO_UINT64
/* Convert float to unsigned int64
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
uint64_t OPTIMIZED float32_to_uint64(float32 a)
{
	if (isFloat32NaN(a)) {
		return UINT64_MAX;
	}
	
	if (isFloat32Infinity(a) || (a.parts.exp >= (64 + FLOAT32_BIAS)))  {
		if (a.parts.sign) {
			return UINT64_MIN;
		}
		return UINT64_MAX;
	}
	
	return _float32_to_uint64_helper(a);	
}
#endif	// _NEEDS_FLOAT32_TO_UINT64


#ifdef _NEEDS_FLOAT32_TO_INT64
/* Convert float to signed int64
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
int64_t OPTIMIZED float32_to_int64(float32 a)
{
	if (isFloat32NaN(a)) {
		return INT64_MAX;
	}
	
	if (isFloat32Infinity(a) || (a.parts.exp >= (64 + FLOAT32_BIAS)))  {
		if (a.parts.sign) {
			return (INT64_MIN);
		}
		return INT64_MAX;
	}
	return _float32_to_uint64_helper(a);
}	
#endif	// _NEEDS_FLOAT32_TO_INT64


#ifdef _NEEDS_FLOAT64_TO_UINT32
/* Convert float64 to unsigned int32
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
uint32_t OPTIMIZED float64_to_uint32(float64 a)
{
	if (isFloat64NaN(a)) {
		return UINT32_MAX;
	}
	
	if (isFloat64Infinity(a) || (a.parts.exp >= (32 + FLOAT64_BIAS)))  {
		if (a.parts.sign) {
			return UINT32_MIN;
		}
		return UINT32_MAX;
	}
	
	return (uint32_t)_float64_to_uint64_helper(a);	
}
#endif	// _NEEDS_FLOAT64_TO_UINT32


#ifdef _NEEDS_FLOAT64_TO_INT32
/* Convert float64 to signed int32
 * FIXME: Im not sure what to return if overflow/underflow happens 
 * 	- now its the biggest or the smallest int
 */ 
int32_t OPTIMIZED float64_to_int32(float64 a)
{
	if (isFloat64NaN(a)) {
		return INT32_MAX;
	}
	
	if (isFloat64Infinity(a) || (a.parts.exp >= (32 + FLOAT64_BIAS)))  {
		if (a.parts.sign) {
			return INT32_MIN;
		}
		return INT32_MAX;
	}
	return (int32_t)_float64_to_uint64_helper(a);
}	
#endif	// _NEEDS_FLOAT64_TO_INT32


#ifdef _NEEDS_UINT32_TO_FLOAT32
/** Convert unsigned integer to float32
 */
float32 OPTIMIZED uint32_to_float32(uint32_t i)
{
	int counter;
	int32_t exp;
	float32 result;
	
	result.parts.sign = 0;
	result.parts.fraction = 0;

	counter = __clzsi2(i);

	exp = FLOAT32_BIAS + 32 - counter - 1;
	
	if (counter == 32) {
		result.binary = 0;
		return result;
	}
	
	if (counter > 0) {
		i <<= counter - 1;
	} else {
		i >>= 1;
	}

	roundFloat32(&exp, &i);

	result.parts.fraction = i >> 7;
	result.parts.exp = exp;

	return result;
}
#endif	// _NEEDS_UINT32_TO_FLOAT32


#ifdef _NEEDS_INT32_TO_FLOAT32
float32 OPTIMIZED int32_to_float32(int32_t i)
{
	float32 result;

	if (i < 0) {
		result = uint32_to_float32((uint32_t)(-i));
	} else {
		result = uint32_to_float32((uint32_t)i);
	}
	
	result.parts.sign = i < 0;

	return result;
}
#endif	// _NEEDS_INT32_TO_FLOAT32


#ifdef _NEEDS_UINT64_TO_FLOAT32
float32 OPTIMIZED uint64_to_float32(uint64_t i)
{
	int counter;
	int32_t exp;
	uint32_t j;
	float32 result;
	
	result.parts.sign = 0;
	result.parts.fraction = 0;

	counter = __clzdi2(i);

	exp = FLOAT32_BIAS + 64 - counter - 1;
	
	if (counter == 64) {
		result.binary = 0;
		return result;
	}
	
	/* Shift all to the first 31 bits (31. will be hidden 1)*/
	if (counter > 33) {
		i <<= counter - 1 - 32;
	} else {
		i >>= 1 + 32 - counter;
	}
	
	j = (uint32_t)i;
	roundFloat32(&exp, &j);

	result.parts.fraction = j >> 7;
	result.parts.exp = exp;
	return result;
}
#endif // _NEEDS_UINT64_TO_FLOAT32


#ifdef _NEEDS_INT64_TO_FLOAT32
float32 OPTIMIZED int64_to_float32(int64_t i)
{
	float32 result;

	if (i < 0) {
		result = uint64_to_float32((uint64_t)(-i));
	} else {
		result = uint64_to_float32((uint64_t)i);
	}
	
	result.parts.sign = i < 0;

	return result;
}
#endif	// _NEEDS_INT64_TO_FLOAT32


#ifdef _NEEDS_UINT32_TO_FLOAT64
/** Convert unsigned integer to float64
 */
float64 OPTIMIZED uint32_to_float64(uint32_t i)
{
	int counter;
	int32_t exp;
	float64 result;
	uint64_t frac;
	
	result.parts.sign = 0;
	result.parts.fraction = 0;

	counter = __clzsi2(i);

	exp = FLOAT64_BIAS + 32 - counter - 1;
	
	if (counter == 32) {
		result.binary = 0;
		return result;
	}
	
	frac = i;
	frac <<= counter + 32 - 1; 

	roundFloat64(&exp, &frac);

	result.parts.fraction = frac >> 10;
	result.parts.exp = exp;

	return result;
}
#endif // _NEEDS_UINT32_TO_FLOAT64


#ifdef _NEEDS_INT32_TO_FLOAT64
float64 OPTIMIZED int32_to_float64(int32_t i)
{
	float64 result;

	if (i < 0) {
		result = uint32_to_float64((uint32_t)(-i));
	} else {
		result = uint32_to_float64((uint32_t)i);
	}
	
	result.parts.sign = i < 0;

	return result;
}
#endif	// _NEEDS_INT32_TO_FLOAT64


#ifdef _NEEDS_UINT64_TO_FLOAT64
float64 OPTIMIZED uint64_to_float64(uint64_t i)
{
	int counter;
	int32_t exp;
	float64 result;
	
	result.parts.sign = 0;
	result.parts.fraction = 0;

	counter = __clzdi2(i);

	exp = FLOAT64_BIAS + 64 - counter - 1;
	
	if (counter == 64) {
		result.binary = 0;
		return result;
	}
	
	if (counter > 0) {
		i <<= counter - 1;
	} else {
		i >>= 1;
	}

	roundFloat64(&exp, &i);

	result.parts.fraction = i >> 10;
	result.parts.exp = exp;
	return result;
}
#endif	// _NEEDS_UINT64_TO_FLOAT64


#ifdef _NEEDS_INT64_TO_FLOAT64
float64 OPTIMIZED int64_to_float64(int64_t i)
{
	float64 result;

	if (i < 0) {
		result = uint64_to_float64((uint64_t)(-i));
	} else {
		result = uint64_to_float64((uint64_t)i);
	}
	
	result.parts.sign = i < 0;

	return result;
}
#endif	// _NEEDS_INT64_TO_FLOAT64

 
#ifdef _NEEDS_FINISHFLOAT64
/** Take fraction shifted by 10 bits to left, round it, normalize it and detect exceptions
 * @param cexp exponent with bias
 * @param cfrac fraction shifted 10 places left with added hidden bit
 * @param sign
 * @return valied float64
 */
float64 OPTIMIZED finishFloat64(int32_t cexp, uint64_t cfrac, char sign)
{
	float64 result;

	result.parts.sign = sign;

	/* find first nonzero digit and shift result and detect possibly underflow */
	while ((cexp > 0) && (cfrac) && (!(cfrac & (FLOAT64_HIDDEN_BIT_MASK << (64 - FLOAT64_FRACTION_SIZE - 1 ) )))) {
		cexp--; 
		cfrac <<= 1;
			/* TODO: fix underflow */
	};
	
	if ((cexp < 0) || ( cexp == 0 && (!(cfrac & (FLOAT64_HIDDEN_BIT_MASK << (64 - FLOAT64_FRACTION_SIZE - 1)))))) {
		/* FIXME: underflow */
		result.parts.exp = 0;
		if ((cexp + FLOAT64_FRACTION_SIZE + 1) < 0) { /* +1 is place for rounding */
			result.parts.fraction = 0;
			return result;
		}
		
		while (cexp < 0) {
			cexp++;
			cfrac >>= 1;
		}
	
		cfrac += (0x1 << (64 - FLOAT64_FRACTION_SIZE - 3)); 
		
		if (!(cfrac & (FLOAT64_HIDDEN_BIT_MASK << (64 - FLOAT64_FRACTION_SIZE - 1)))) {
			
			result.parts.fraction = ((cfrac >>(64 - FLOAT64_FRACTION_SIZE - 2) ) & (~FLOAT64_HIDDEN_BIT_MASK)); 
			return result;
		}	
	} else {
		cfrac += (0x1 << (64 - FLOAT64_FRACTION_SIZE - 3)); 
	}
	
	++cexp;

	if (cfrac & (FLOAT64_HIDDEN_BIT_MASK << (64 - FLOAT64_FRACTION_SIZE - 1 ))) {
		++cexp;
		cfrac >>= 1;
	}	

	/* check overflow */
	if (cexp >= FLOAT64_MAX_EXPONENT ) {
		/* FIXME: overflow, return infinity */
		result.parts.exp = FLOAT64_MAX_EXPONENT;
		result.parts.fraction = 0;
		return result;
	}

	result.parts.exp = (uint32_t)cexp;
	
	result.parts.fraction = ((cfrac >>(64 - FLOAT64_FRACTION_SIZE - 2 ) ) & (~FLOAT64_HIDDEN_BIT_MASK)); 
	
	return result;	
}
#endif	// _NEEDS_FINISHFLOAT64


#ifdef _NEEDS_SUBFLOAT32
/** Subtract two float32 numbers with same signs
 */
float32 OPTIMIZED subFloat32(float32 a, float32 b)
{
	int expdiff;
	uint64_t exp1, exp2, frac1, frac2;
	float32 result;

	result.f = 0;
	
	expdiff = a.parts.exp - b.parts.exp;
	if ((expdiff < 0 ) || ((expdiff == 0) && (a.parts.fraction < b.parts.fraction))) {
		if (isFloat32NaN(b)) {
			/* TODO: fix SigNaN */
			if (isFloat32SigNaN(b)) {
			};
			return b;
		};
		
		if (b.parts.exp == FLOAT32_MAX_EXPONENT) { 
			b.parts.sign = !b.parts.sign; /* num -(+-inf) = -+inf */
			return b;
		}
		
		result.parts.sign = !a.parts.sign; 
		
		frac1 = b.parts.fraction;
		exp1 = b.parts.exp;
		frac2 = a.parts.fraction;
		exp2 = a.parts.exp;
		expdiff *= -1;
	} else {
		if (isFloat32NaN(a)) {
			/* TODO: fix SigNaN */
			if (isFloat32SigNaN(a) || isFloat32SigNaN(b)) {
			};
			return a;
		};
		
		if (a.parts.exp == FLOAT32_MAX_EXPONENT) { 
			if (b.parts.exp == FLOAT32_MAX_EXPONENT) {
				/* inf - inf => nan */
				/* TODO: fix exception */
				result.binary = FLOAT32_NAN;
				return result;
			};
			return a;
		}
		
		result.parts.sign = a.parts.sign;
		
		frac1 = a.parts.fraction;
		exp1 = a.parts.exp;
		frac2 = b.parts.fraction;
		exp2 = b.parts.exp;	
	};
	
	if (exp1 == 0) {
		/* both are denormalized */
		result.parts.fraction = frac1-frac2;
		if (result.parts.fraction > frac1) {
			/* TODO: underflow exception */
			return result;
		};
		result.parts.exp = 0;
		return result;
	};

	/* add hidden bit */
	frac1 |= FLOAT32_HIDDEN_BIT_MASK; 
	
	if (exp2 == 0) {
		/* denormalized */
		--expdiff;	
	} else {
		/* normalized */
		frac2 |= FLOAT32_HIDDEN_BIT_MASK;
	};
	
	/* create some space for rounding */
	frac1 <<= 6;
	frac2 <<= 6;
	
	if (expdiff > FLOAT32_FRACTION_SIZE + 1) {
		 goto done;	
		 };
	
	frac1 = frac1 - (frac2 >> expdiff);
done:
	/* TODO: find first nonzero digit and shift result and detect possibly underflow */
	while ((exp1 > 0) && (!(frac1 & (FLOAT32_HIDDEN_BIT_MASK << 6 )))) {
		--exp1;
		frac1 <<= 1;
			/* TODO: fix underflow - frac1 == 0 does not necessary means underflow... */
	};
	
	/* rounding - if first bit after fraction is set then round up */
	frac1 += 0x20;

	if (frac1 & (FLOAT32_HIDDEN_BIT_MASK << 7)) {
		++exp1;
		frac1 >>= 1;
	};
	
	/*Clear hidden bit and shift */
	result.parts.fraction = ((frac1 >> 6) & (~FLOAT32_HIDDEN_BIT_MASK)); 
	result.parts.exp = exp1;
	
	return result;
}
#endif	// _NEEDS_SUBFLOAT32


#ifdef _NEEDS_SUBFLOAT64
/** Subtract two float64 numbers with same signs
 */
float64 OPTIMIZED subFloat64(float64 a, float64 b)
{
	int expdiff;
	uint64_t exp1, exp2;
	uint64_t frac1, frac2;
	float64 result;

	result.d = 0;
	
	expdiff = a.parts.exp - b.parts.exp;
	if ((expdiff < 0 ) || ((expdiff == 0) && (a.parts.fraction < b.parts.fraction))) {
		if (isFloat64NaN(b)) {
			/* TODO: fix SigNaN */
			if (isFloat64SigNaN(b)) {
			};
			return b;
		};
		
		if (b.parts.exp == FLOAT64_MAX_EXPONENT) { 
			b.parts.sign = !b.parts.sign; /* num -(+-inf) = -+inf */
			return b;
		}
		
		result.parts.sign = !a.parts.sign; 
		
		frac1 = b.parts.fraction;
		exp1 = b.parts.exp;
		frac2 = a.parts.fraction;
		exp2 = a.parts.exp;
		expdiff *= -1;
	} else {
		if (isFloat64NaN(a)) {
			/* TODO: fix SigNaN */
			if (isFloat64SigNaN(a) || isFloat64SigNaN(b)) {
			};
			return a;
		};
		
		if (a.parts.exp == FLOAT64_MAX_EXPONENT) { 
			if (b.parts.exp == FLOAT64_MAX_EXPONENT) {
				/* inf - inf => nan */
				/* TODO: fix exception */
				result.binary = FLOAT64_NAN;
				return result;
			};
			return a;
		}
		
		result.parts.sign = a.parts.sign;
		
		frac1 = a.parts.fraction;
		exp1 = a.parts.exp;
		frac2 = b.parts.fraction;
		exp2 = b.parts.exp;	
	};
	
	if (exp1 == 0) {
		/* both are denormalized */
		result.parts.fraction = frac1 - frac2;
		if (result.parts.fraction > frac1) {
			/* TODO: underflow exception */
			return result;
		};
		result.parts.exp = 0;
		return result;
	};

	/* add hidden bit */
	frac1 |= FLOAT64_HIDDEN_BIT_MASK; 
	
	if (exp2 == 0) {
		/* denormalized */
		--expdiff;	
	} else {
		/* normalized */
		frac2 |= FLOAT64_HIDDEN_BIT_MASK;
	};
	
	/* create some space for rounding */
	frac1 <<= 6;
	frac2 <<= 6;
	
	if (expdiff > FLOAT64_FRACTION_SIZE + 1) {
		 goto done;	
		 };
	
	frac1 = frac1 - (frac2 >> expdiff);
done:
	/* TODO: find first nonzero digit and shift result and detect possibly underflow */
	while ((exp1 > 0) && (!(frac1 & (FLOAT64_HIDDEN_BIT_MASK << 6 )))) {
		--exp1;
		frac1 <<= 1;
			/* TODO: fix underflow - frac1 == 0 does not necessary means underflow... */
	};
	
	/* rounding - if first bit after fraction is set then round up */
	frac1 += 0x20;

	if (frac1 & (FLOAT64_HIDDEN_BIT_MASK << 7)) {
		++exp1;
		frac1 >>= 1;
	};
	
	/*Clear hidden bit and shift */
	result.parts.fraction = ((frac1 >> 6) & (~FLOAT64_HIDDEN_BIT_MASK)); 
	result.parts.exp = exp1;
	
	return result;
}
#endif	// _NEEDS_SUBFLOAT64


#ifdef _NEEDS_ADDFLOAT32
/** Add two Float32 numbers with same signs
 */
float32 OPTIMIZED addFloat32(float32 a, float32 b)
{
	int expdiff;
	uint32_t exp1, exp2,frac1, frac2;
	
	expdiff = a.parts.exp - b.parts.exp;
	if (expdiff < 0) {
		if (isFloat32NaN(b)) {
			/* TODO: fix SigNaN */
			if (isFloat32SigNaN(b)) {
			};

			return b;
		};
		
		if (b.parts.exp == FLOAT32_MAX_EXPONENT) { 
			return b;
		}
		
		frac1 = b.parts.fraction;
		exp1 = b.parts.exp;
		frac2 = a.parts.fraction;
		exp2 = a.parts.exp;
		expdiff *= -1;
	} else {
		if ((isFloat32NaN(a)) || (isFloat32NaN(b))) {
			/* TODO: fix SigNaN */
			if (isFloat32SigNaN(a) || isFloat32SigNaN(b)) {
			};
			return (isFloat32NaN(a)?a:b);
		};
		
		if (a.parts.exp == FLOAT32_MAX_EXPONENT) { 
			return a;
		}
		
		frac1 = a.parts.fraction;
		exp1 = a.parts.exp;
		frac2 = b.parts.fraction;
		exp2 = b.parts.exp;
	};
	
	if (exp1 == 0) {
		/* both are denormalized */
		frac1 += frac2;
		if (frac1 & FLOAT32_HIDDEN_BIT_MASK ) {
			/* result is not denormalized */
			a.parts.exp = 1;
		};
		a.parts.fraction = frac1;
		return a;
	};
	
	frac1 |= FLOAT32_HIDDEN_BIT_MASK; /* add hidden bit */

	if (exp2 == 0) {
		/* second operand is denormalized */
		--expdiff;
	} else {
		/* add hidden bit to second operand */
		frac2 |= FLOAT32_HIDDEN_BIT_MASK; 
	};
	
	/* create some space for rounding */
	frac1 <<= 6;
	frac2 <<= 6;
	
	if (expdiff < (FLOAT32_FRACTION_SIZE + 2) ) {
		frac2 >>= expdiff;
		frac1 += frac2;
	} else {
		a.parts.exp = exp1;
		a.parts.fraction = (frac1 >> 6) & (~(FLOAT32_HIDDEN_BIT_MASK));
		return a;
	}
	
	if (frac1 & (FLOAT32_HIDDEN_BIT_MASK << 7) ) {
		++exp1;
		frac1 >>= 1;
	};
	
	/* rounding - if first bit after fraction is set then round up */
	frac1 += (0x1 << 5);
	
	if (frac1 & (FLOAT32_HIDDEN_BIT_MASK << 7)) { 
		/* rounding overflow */
		++exp1;
		frac1 >>= 1;
	};
	
	
	if ((exp1 == FLOAT32_MAX_EXPONENT ) || (exp2 > exp1)) {
			/* overflow - set infinity as result */
			a.parts.exp = FLOAT32_MAX_EXPONENT;
			a.parts.fraction = 0;
			return a;
			}
	
	a.parts.exp = exp1;
	
	/*Clear hidden bit and shift */
	a.parts.fraction = ((frac1 >> 6) & (~FLOAT32_HIDDEN_BIT_MASK)) ; 
	return a;
}
#endif	// _NEEDS_ADDFLOAT32


#ifdef _NEEDS_ADDFLOAT64
/** Add two Float64 numbers with same signs
 */
float64 OPTIMIZED addFloat64(float64 a, float64 b)
{
	int expdiff;
	uint32_t exp1, exp2;
	uint64_t frac1, frac2;
	
	expdiff = ((int )a.parts.exp) - b.parts.exp;
	if (expdiff < 0) {
		if (isFloat64NaN(b)) {
			/* TODO: fix SigNaN */
			if (isFloat64SigNaN(b)) {
			};

			return b;
		};
		
		/* b is infinity and a not */	
		if (b.parts.exp == FLOAT64_MAX_EXPONENT ) { 
			return b;
		}
		
		frac1 = b.parts.fraction;
		exp1 = b.parts.exp;
		frac2 = a.parts.fraction;
		exp2 = a.parts.exp;
		expdiff *= -1;
	} else {
		if (isFloat64NaN(a)) {
			/* TODO: fix SigNaN */
			if (isFloat64SigNaN(a) || isFloat64SigNaN(b)) {
			};
			return a;
		};
		
		/* a is infinity and b not */
		if (a.parts.exp == FLOAT64_MAX_EXPONENT ) { 
			return a;
		}
		
		frac1 = a.parts.fraction;
		exp1 = a.parts.exp;
		frac2 = b.parts.fraction;
		exp2 = b.parts.exp;
	};
	
	if (exp1 == 0) {
		/* both are denormalized */
		frac1 += frac2;
		if (frac1 & FLOAT64_HIDDEN_BIT_MASK) { 
			/* result is not denormalized */
			a.parts.exp = 1;
		};
		a.parts.fraction = frac1;
		return a;
	};
	
	/* add hidden bit - frac1 is sure not denormalized */
	frac1 |= FLOAT64_HIDDEN_BIT_MASK;

	/* second operand ... */
	if (exp2 == 0) {
		/* ... is denormalized */
		--expdiff;	
	} else {
		/* is not denormalized */
		frac2 |= FLOAT64_HIDDEN_BIT_MASK;
	};
	
	/* create some space for rounding */
	frac1 <<= 6;
	frac2 <<= 6;
	
	if (expdiff < (FLOAT64_FRACTION_SIZE + 2) ) {
		frac2 >>= expdiff;
		frac1 += frac2;
	} else {
		a.parts.exp = exp1;
		a.parts.fraction = (frac1 >> 6) & (~(FLOAT64_HIDDEN_BIT_MASK));
		return a;
	}
	
	if (frac1 & (FLOAT64_HIDDEN_BIT_MASK << 7) ) {
		++exp1;
		frac1 >>= 1;
	};
	
	/* rounding - if first bit after fraction is set then round up */
	frac1 += (0x1 << 5); 
	
	if (frac1 & (FLOAT64_HIDDEN_BIT_MASK << 7)) { 
		/* rounding overflow */
		++exp1;
		frac1 >>= 1;
	};
	
	if ((exp1 == FLOAT64_MAX_EXPONENT ) || (exp2 > exp1)) {
			/* overflow - set infinity as result */
			a.parts.exp = FLOAT64_MAX_EXPONENT;
			a.parts.fraction = 0;
			return a;
			}
	
	a.parts.exp = exp1;
	/*Clear hidden bit and shift */
	a.parts.fraction = ( (frac1 >> 6 ) & (~FLOAT64_HIDDEN_BIT_MASK));
	
	return a;
}
#endif	// _NEEDS_ADDFLOAT64


#ifdef _NEEDS_MULFLOAT32
/** Multiply two 32 bit float numbers
 *
 */
float32 OPTIMIZED mulFloat32(float32 a, float32 b)
{
	float32 result;
	uint64_t frac1, frac2;
	int32_t exp;

	result.parts.sign = a.parts.sign ^ b.parts.sign;
	
	if (isFloat32NaN(a) || isFloat32NaN(b) ) {
		/* TODO: fix SigNaNs */
		if (isFloat32SigNaN(a)) {
			result.parts.fraction = a.parts.fraction;
			result.parts.exp = a.parts.exp;
			return result;
		};
		if (isFloat32SigNaN(b)) { /* TODO: fix SigNaN */
			result.parts.fraction = b.parts.fraction;
			result.parts.exp = b.parts.exp;
			return result;
		};
		/* set NaN as result */
		result.binary = FLOAT32_NAN;
		return result;
	};
		
	if (isFloat32Infinity(a)) { 
		if (isFloat32Zero(b)) {
			/* FIXME: zero * infinity */
			result.binary = FLOAT32_NAN;
			return result;
		}
		result.parts.fraction = a.parts.fraction;
		result.parts.exp = a.parts.exp;
		return result;
	}

	if (isFloat32Infinity(b)) { 
		if (isFloat32Zero(a)) {
			/* FIXME: zero * infinity */
			result.binary = FLOAT32_NAN;
			return result;
		}
		result.parts.fraction = b.parts.fraction;
		result.parts.exp = b.parts.exp;
		return result;
	}

	/* exp is signed so we can easy detect underflow */
	exp = a.parts.exp + b.parts.exp;
	exp -= FLOAT32_BIAS;
	
	if (exp >= FLOAT32_MAX_EXPONENT) {
		/* FIXME: overflow */
		/* set infinity as result */
		result.binary = FLOAT32_INF;
		result.parts.sign = a.parts.sign ^ b.parts.sign;
		return result;
	};
	
	if (exp < 0) { 
		/* FIXME: underflow */
		/* return signed zero */
		result.parts.fraction = 0x0;
		result.parts.exp = 0x0;
		return result;
	};
	
	frac1 = a.parts.fraction;
	if (a.parts.exp > 0) {
		frac1 |= FLOAT32_HIDDEN_BIT_MASK;
	} else {
		++exp;
	};
	
	frac2 = b.parts.fraction;

	if (b.parts.exp > 0) {
		frac2 |= FLOAT32_HIDDEN_BIT_MASK;
	} else {
		++exp;
	};

	frac1 <<= 1; /* one bit space for rounding */

	frac1 = frac1 * frac2;
/* round and return */
	
	while ((exp < FLOAT32_MAX_EXPONENT) && (frac1 >= ( 1 << (FLOAT32_FRACTION_SIZE + 2)))) { 
		/* 23 bits of fraction + one more for hidden bit (all shifted 1 bit left)*/
		++exp;
		frac1 >>= 1;
	};

	/* rounding */
	/* ++frac1; FIXME: not works - without it is ok */
	frac1 >>= 1; /* shift off rounding space */
	
	if ((exp < FLOAT32_MAX_EXPONENT) && (frac1 >= (1 << (FLOAT32_FRACTION_SIZE + 1)))) {
		++exp;
		frac1 >>= 1;
	};

	if (exp >= FLOAT32_MAX_EXPONENT ) {	
		/* TODO: fix overflow */
		/* return infinity*/
		result.parts.exp = FLOAT32_MAX_EXPONENT;
		result.parts.fraction = 0x0;
		return result;
	}
	
	exp -= FLOAT32_FRACTION_SIZE;

	if (exp <= FLOAT32_FRACTION_SIZE) { 
		/* denormalized number */
		frac1 >>= 1; /* denormalize */
		while ((frac1 > 0) && (exp < 0)) {
			frac1 >>= 1;
			++exp;
		};
		if (frac1 == 0) {
			/* FIXME : underflow */
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
		};
	};
	result.parts.exp = exp; 
	result.parts.fraction = frac1 & ( (1 << FLOAT32_FRACTION_SIZE) - 1);
	
	return result;	
	
}
#endif	// _NEEDS_MULFLOAT32


#ifdef _NEEDS_MUL64INTEGERS
/** Multiply two 64 bit numbers and return result in two parts
 * @param a first operand
 * @param b second operand
 * @param lo lower part from result
 * @param hi higher part of result
 */
void OPTIMIZED mul64integers(uint64_t a,uint64_t b, uint64_t *lo, uint64_t *hi)
{
	uint64_t low, high, middle1, middle2;
	uint32_t alow, blow;

	alow = a & 0xFFFFFFFF;
	blow = b & 0xFFFFFFFF;
	
	a >>= 32;
	b >>= 32;
	
	low = ((uint64_t)alow) * blow;
	middle1 = a * blow;
	middle2 = alow * b;
	high = a * b;

	middle1 += middle2;
	high += (((uint64_t)(middle1 < middle2)) << 32) + (middle1 >> 32);
	middle1 <<= 32;
	low += middle1;
	high += (low < middle1);
	*lo = low;
	*hi = high;
	
	return;
} 
#endif	// _NEEDS_MUL64INTEGERS


#ifdef _NEEDS_MULFLOAT64
/** Multiply two 64 bit float numbers
 *
 */
float64 OPTIMIZED mulFloat64(float64 a, float64 b)
{
	float64 result;
	uint64_t frac1, frac2;
	int32_t exp;

	result.parts.sign = a.parts.sign ^ b.parts.sign;
	
	if (isFloat64NaN(a) || isFloat64NaN(b) ) {
		/* TODO: fix SigNaNs */
		if (isFloat64SigNaN(a)) {
			result.parts.fraction = a.parts.fraction;
			result.parts.exp = a.parts.exp;
			return result;
		};
		if (isFloat64SigNaN(b)) { /* TODO: fix SigNaN */
			result.parts.fraction = b.parts.fraction;
			result.parts.exp = b.parts.exp;
			return result;
		};
		/* set NaN as result */
		result.binary = FLOAT64_NAN;
		return result;
	};
		
	if (isFloat64Infinity(a)) { 
		if (isFloat64Zero(b)) {
			/* FIXME: zero * infinity */
			result.binary = FLOAT64_NAN;
			return result;
		}
		result.parts.fraction = a.parts.fraction;
		result.parts.exp = a.parts.exp;
		return result;
	}

	if (isFloat64Infinity(b)) { 
		if (isFloat64Zero(a)) {
			/* FIXME: zero * infinity */
			result.binary = FLOAT64_NAN;
			return result;
		}
		result.parts.fraction = b.parts.fraction;
		result.parts.exp = b.parts.exp;
		return result;
	}

	/* exp is signed so we can easy detect underflow */
	exp = a.parts.exp + b.parts.exp - FLOAT64_BIAS;
	
	frac1 = a.parts.fraction;

	if (a.parts.exp > 0) {
		frac1 |= FLOAT64_HIDDEN_BIT_MASK;
	} else {
		++exp;
	};
	
	frac2 = b.parts.fraction;

	if (b.parts.exp > 0) {
		frac2 |= FLOAT64_HIDDEN_BIT_MASK;
	} else {
		++exp;
	};

	frac1 <<= (64 - FLOAT64_FRACTION_SIZE - 1);
	frac2 <<= (64 - FLOAT64_FRACTION_SIZE - 2);

	mul64integers(frac1, frac2, &frac1, &frac2);

	frac2 |= (frac1 != 0);
	if (frac2 & (0x1ll << 62)) {
		frac2 <<= 1;
		exp--;
	}

	result = finishFloat64(exp, frac2, result.parts.sign);
	return result;
}
#endif	// _NEEDS_MULFLOAT64


#ifdef _NEEDS_DIVFLOAT32
float32 OPTIMIZED divFloat32(float32 a, float32 b)
{
	float32 result;
	int32_t aexp, bexp, cexp;
	uint64_t afrac, bfrac, cfrac;
	
	result.parts.sign = a.parts.sign ^ b.parts.sign;
	
	if (isFloat32NaN(a)) {
		if (isFloat32SigNaN(a)) {
			/*FIXME: SigNaN*/
		}
		/*NaN*/
		return a;
	}
	
	if (isFloat32NaN(b)) {
		if (isFloat32SigNaN(b)) {
			/*FIXME: SigNaN*/
		}
		/*NaN*/
		return b;
	}
	
	if (isFloat32Infinity(a)) {
		if (isFloat32Infinity(b)) {
			/*FIXME: inf / inf */
			result.binary = FLOAT32_NAN;
			return result;
		}
		/* inf / num */
		result.parts.exp = a.parts.exp;
		result.parts.fraction = a.parts.fraction;
		return result;
	}

	if (isFloat32Infinity(b)) {
		if (isFloat32Zero(a)) {
			/* FIXME 0 / inf */
			result.parts.exp = 0;
			result.parts.fraction = 0;
			return result;
		}
		/* FIXME: num / inf*/
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
	}
	
	if (isFloat32Zero(b)) {
		if (isFloat32Zero(a)) {
			/*FIXME: 0 / 0*/
			result.binary = FLOAT32_NAN;
			return result;
		}
		/* FIXME: division by zero */
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
	}

	
	afrac = a.parts.fraction;
	aexp = a.parts.exp;
	bfrac = b.parts.fraction;
	bexp = b.parts.exp;
	
	/* denormalized numbers */
	if (aexp == 0) {
		if (afrac == 0) {
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
		}
		/* normalize it*/
		
		afrac <<= 1;
			/* afrac is nonzero => it must stop */	
		while (! (afrac & FLOAT32_HIDDEN_BIT_MASK) ) {
			afrac <<= 1;
			aexp--;
		}
	}

	if (bexp == 0) {
		bfrac <<= 1;
			/* bfrac is nonzero => it must stop */	
		while (! (bfrac & FLOAT32_HIDDEN_BIT_MASK) ) {
			bfrac <<= 1;
			bexp--;
		}
	}

	afrac =	(afrac | FLOAT32_HIDDEN_BIT_MASK ) << (32 - FLOAT32_FRACTION_SIZE - 1 );
	bfrac =	(bfrac | FLOAT32_HIDDEN_BIT_MASK ) << (32 - FLOAT32_FRACTION_SIZE );

	if ( bfrac <= (afrac << 1) ) {
		afrac >>= 1;
		aexp++;
	}
	
	cexp = aexp - bexp + FLOAT32_BIAS - 2;
	
	cfrac = (afrac << 32) / bfrac;
	if ((  cfrac & 0x3F ) == 0) { 
		cfrac |= ( bfrac * cfrac != afrac << 32 );
	}
	
	/* pack and round */
	
	/* find first nonzero digit and shift result and detect possibly underflow */
	while ((cexp > 0) && (cfrac) && (!(cfrac & (FLOAT32_HIDDEN_BIT_MASK << 7 )))) {
		cexp--;
		cfrac <<= 1;
			/* TODO: fix underflow */
	};
	
	cfrac += (0x1 << 6); /* FIXME: 7 is not sure*/
	
	if (cfrac & (FLOAT32_HIDDEN_BIT_MASK << 7)) {
		++cexp;
		cfrac >>= 1;
		}	

	/* check overflow */
	if (cexp >= FLOAT32_MAX_EXPONENT ) {
		/* FIXME: overflow, return infinity */
		result.parts.exp = FLOAT32_MAX_EXPONENT;
		result.parts.fraction = 0;
		return result;
	}

	if (cexp < 0) {
		/* FIXME: underflow */
		result.parts.exp = 0;
		if ((cexp + FLOAT32_FRACTION_SIZE) < 0) {
			result.parts.fraction = 0;
			return result;
		}
		cfrac >>= 1;
		while (cexp < 0) {
			cexp ++;
			cfrac >>= 1;
		}
		
	} else {
		result.parts.exp = (uint32_t)cexp;
	}
	
	result.parts.fraction = ((cfrac >> 6) & (~FLOAT32_HIDDEN_BIT_MASK)); 
	
	return result;	
}
#endif	// _NEEDS_DIVFLOAT32


#ifdef _NEEDS_DIVFLOAT64
float64 OPTIMIZED divFloat64(float64 a, float64 b)
{
	float64 result;
	int64_t aexp, bexp, cexp;
	uint64_t afrac, bfrac, cfrac; 
	uint64_t remlo, remhi;
	
	result.parts.sign = a.parts.sign ^ b.parts.sign;
	
	if (isFloat64NaN(a)) {
		
		if (isFloat64SigNaN(b)) {
			/*FIXME: SigNaN*/
			return b;
		}
		
		if (isFloat64SigNaN(a)) {
			/*FIXME: SigNaN*/
		}
		/*NaN*/
		return a;
	}
	
	if (isFloat64NaN(b)) {
		if (isFloat64SigNaN(b)) {
			/*FIXME: SigNaN*/
		}
		/*NaN*/
		return b;
	}
	
	if (isFloat64Infinity(a)) {
		if (isFloat64Infinity(b) || isFloat64Zero(b)) {
			/*FIXME: inf / inf */
			result.binary = FLOAT64_NAN;
			return result;
		}
		/* inf / num */
		result.parts.exp = a.parts.exp;
		result.parts.fraction = a.parts.fraction;
		return result;
	}

	if (isFloat64Infinity(b)) {
		if (isFloat64Zero(a)) {
			/* FIXME 0 / inf */
			result.parts.exp = 0;
			result.parts.fraction = 0;
			return result;
		}
		/* FIXME: num / inf*/
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
	}
	
	if (isFloat64Zero(b)) {
		if (isFloat64Zero(a)) {
			/*FIXME: 0 / 0*/
			result.binary = FLOAT64_NAN;
			return result;
		}
		/* FIXME: division by zero */
		result.parts.exp = 0;
		result.parts.fraction = 0;
		return result;
	}

	
	afrac = a.parts.fraction;
	aexp = a.parts.exp;
	bfrac = b.parts.fraction;
	bexp = b.parts.exp;
	
	/* denormalized numbers */
	if (aexp == 0) {
		if (afrac == 0) {
			result.parts.exp = 0;
			result.parts.fraction = 0;
			return result;
		}
		/* normalize it*/
		
		aexp++;
			/* afrac is nonzero => it must stop */	
		while (! (afrac & FLOAT64_HIDDEN_BIT_MASK) ) {
			afrac <<= 1;
			aexp--;
		}
	}

	if (bexp == 0) {
		bexp++;
			/* bfrac is nonzero => it must stop */	
		while (! (bfrac & FLOAT64_HIDDEN_BIT_MASK) ) {
			bfrac <<= 1;
			bexp--;
		}
	}

	afrac =	(afrac | FLOAT64_HIDDEN_BIT_MASK ) << (64 - FLOAT64_FRACTION_SIZE - 2 );
	bfrac =	(bfrac | FLOAT64_HIDDEN_BIT_MASK ) << (64 - FLOAT64_FRACTION_SIZE - 1);

	if ( bfrac <= (afrac << 1) ) {
		afrac >>= 1;
		aexp++;
	}
	
	cexp = aexp - bexp + FLOAT64_BIAS - 2; 
	
	cfrac = divFloat64estim(afrac, bfrac);
	
	if ((  cfrac & 0x1FF ) <= 2) { /*FIXME:?? */
		mul64integers( bfrac, cfrac, &remlo, &remhi);
		/* (__u128)afrac << 64 - ( ((__u128)remhi<<64) + (__u128)remlo )*/	
		remhi = afrac - remhi - ( remlo > 0);
		remlo = - remlo;
		
		while ((int64_t) remhi < 0) {
			cfrac--;
			remlo += bfrac;
			remhi += ( remlo < bfrac );
		}
		cfrac |= ( remlo != 0 );
	}
	
	/* round and shift */
	result = finishFloat64(cexp, cfrac, result.parts.sign);
	return result;

}


uint64_t OPTIMIZED divFloat64estim(uint64_t a, uint64_t b)
{
	uint64_t bhi;
	uint64_t remhi, remlo;
	uint64_t result;
	
	if ( b <= a ) {
		return 0xFFFFFFFFFFFFFFFFull;
	}
	
	bhi = b >> 32;
	result = ((bhi << 32) <= a) ?( 0xFFFFFFFFull << 32) : ( a / bhi) << 32;
	mul64integers(b, result, &remlo, &remhi);
	
	remhi = a - remhi - (remlo > 0);
	remlo = - remlo;

	b <<= 32;
	while ( (int64_t) remhi < 0 ) {
			result -= 0x1ll << 32;	
			remlo += b;
			remhi += bhi + ( remlo < b );
		}
	remhi = (remhi << 32) | (remlo >> 32);
	if (( bhi << 32) <= remhi) {
		result |= 0xFFFFFFFF;
	} else {
		result |= remhi / bhi;
	}
	
	return result;
}
#endif	// _NEEDS_DIVFLOAT64


#endif


/*######################################################################################################*/
/* C Floating Point Functions                                                                           */
/*######################################################################################################*/

#if _USE_SOFT_F32_ADD == SIGMATEK_IMPL
float OPTIMIZED __addsf3(float a, float b)
{
	float32 fa, fb;
	fa.f = a;
	fb.f = b;
	if (fa.parts.sign != fb.parts.sign) {
			if (fa.parts.sign) {
					fa.parts.sign = 0;
					return subFloat32(fb, fa).f;
			};
			fb.parts.sign = 0;
			return subFloat32(fa, fb).f;
	}
	return addFloat32(fa, fb).f;
}
#elif _USE_SOFT_F32_ADD == LLVM_ASM_IMPL
void NAKED __addsf3()
{
	asm volatile(
		" .syntax unified \r\n"
		" vmov	s14, r0  \r\n"
		" vmov	s15, r1  \r\n"
		" vadd.f32 s14, s14, s15 \r\n"
		" vmov	r0, s14 \r\n"
		" bx lr \r\n"
		" .syntax divided \r\n"
		: : : "cc");
}
#elif _USE_SOFT_F32_ADD != NO_IMPL
#	error "_USE_SOFT_F32_ADD is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_ADD


#if _USE_SOFT_F64_ADD == SIGMATEK_IMPL
double OPTIMIZED __adddf3(double a, double b)
{
	float64 da, db;
	da.d = a;
	db.d = b;
	if (da.parts.sign != db.parts.sign) {
			if (da.parts.sign) {
					da.parts.sign = 0;
					return subFloat64(db, da).d;
			};
			db.parts.sign = 0;
			return subFloat64(da, db).d;
	}
	return addFloat64(da, db).d;
}
#elif _USE_SOFT_F64_ADD == LLVM_ASM_IMPL
void NAKED __adddf3()
{
	asm volatile(
		" .syntax unified\r\n"
		" vmov	d6, r0, r1 \r\n"
		" vmov	d7, r2, r3 \r\n"
		" vadd.f64 d6, d6, d7\r\n"
		" vmov	r0, r1, d6 \r\n"
		" bx lr \r\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F64_ADD != NO_IMPL
#	error "_USE_SOFT_F64_ADD is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_ADD


#if _USE_SOFT_F32_SUB == SIGMATEK_IMPL
float OPTIMIZED __subsf3(float a, float b)
{
	float32 fa, fb;
	fa.f = a;
	fb.f = b;
	if (fa.parts.sign != fb.parts.sign) {
			fb.parts.sign = !fb.parts.sign;
			return addFloat32(fa, fb).f;
	}
	return subFloat32(fa, fb).f;
}
#elif _USE_SOFT_F32_SUB == LLVM_ASM_IMPL
void NAKED __subsf3(void)
{
	asm volatile(
		" .syntax unified\n"
		" vmov	s14, r0\n"
		" vmov	s15, r1\n"
		" vsub.f32 s14, s14, s15\n"
		" vmov	r0, s14\n"
		" bx	lr\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F32_SUB != NO_IMPL
#	error "_USE_SOFT_F32_SUB is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_SUB


#if _USE_SOFT_F64_SUB == SIGMATEK_IMPL
double OPTIMIZED __subdf3(double a, double b)
{
	float64 da, db;
	da.d = a;
	db.d = b;
	if (da.parts.sign != db.parts.sign) {
			db.parts.sign = !db.parts.sign;
			return addFloat64(da, db).d;
	}
	return subFloat64(da, db).d;
}
#elif _USE_SOFT_F64_SUB == LLVM_ASM_IMPL
void NAKED __subdf3(void)
{
	asm volatile(
		" .syntax unified\r\n"
		" vmov 	d6, r0, r1\r\n"
		" vmov 	d7, r2, r3\r\n"
		" vsub.f64 d6, d6, d7\r\n"
		" vmov 	r0, r1, d6\r\n"
		" bx	lr\r\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F64_SUB != NO_IMPL
#	error "_USE_SOFT_F64_SUB is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_SUB


#if _USE_SOFT_F32_MUL == SIGMATEK_IMPL
float OPTIMIZED __mulsf3(float a, float b)
{
	float32 fa, fb;
	fa.f = a;
	fb.f = b;
	return  mulFloat32(fa, fb).f;
}
#elif _USE_SOFT_F32_MUL == LLVM_ASM_IMPL
void NAKED __mulsf3(void)
{
	float r;
	asm volatile(
		" .syntax unified\n"
		" vmov	s14, r0\n"
		" vmov	s15, r1\n"
		" vmul.f32 s13, s14, s15\n"
		" vmov	r0, s13\n"
		" bx	lr\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F32_MUL != NO_IMPL
#	error "_USE_SOFT_F32_MUL is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_MUL


#if _USE_SOFT_F64_MUL == SIGMATEK_IMPL
double OPTIMIZED __muldf3(double a, double b)
{
	float64 da, db;
	da.d = a;
	db.d = b;
	return  mulFloat64(da, db).d;
}
#elif _USE_SOFT_F64_MUL == LLVM_ASM_IMPL
void OPTIMIZED __muldf3(void)
{
	asm volatile(
		" .syntax unified\r\n"
		" vmov 	d6, r0, r1\r\n"
		" vmov 	d7, r2, r3\r\n"
		" vmul.f64 d6, d6, d7\r\n"
		" vmov 	r0, r1, d6\r\n"
		" bx	lr\r\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F64_MUL != NO_IMPL
#	error "_USE_SOFT_F64_MUL is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_MUL


#if _USE_SOFT_F32_DIV == SIGMATEK_IMPL
float OPTIMIZED __divsf3(float a, float b)
{
	float32 fa, fb;
	fa.f = a;
	fb.f = b;
	return  divFloat32(fa, fb).f;
}
#elif _USE_SOFT_F32_DIV == LLVM_ASM_IMPL
void __attribute__((optimize(2))) __divsf3(float a, float b)
{
	float r;
	if (b == 0.0f)
	{
		THROW_DIV_EXCEPT();
	}
	asm volatile(
		" .syntax unified\n"
		" vdiv.f32 %[result], %[va], %[vb]\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a), [vb] "w" (b) : "cc");
	return r;
	}
#elif _USE_SOFT_F32_DIV != NO_IMPL
#	error "_USE_SOFT_F32_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_DIV


#if _USE_SOFT_F64_DIV == SIGMATEK_IMPL
double OPTIMIZED __divdf3(double a, double b)
{
	float64 da, db;
	da.d = a;
	db.d = b;
	return  divFloat64(da, db).d;
}
#elif _USE_SOFT_F64_DIV == LLVM_ASM_IMPL
double __attribute__((optimize(2))) __divdf3(double a, double b)
{
	double r;
	if (b == 0.0)
	{
		THROW_DIV_EXCEPT();
	}
	asm volatile(
		" .syntax unified\r\n"
		" vdiv.f64 %[result], %[va], %[vb]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a), [vb] "w" (b) : "cc");
	return r;
}
#elif _USE_SOFT_F64_DIV != NO_IMPL
#	error "_USE_SOFT_F64_DIV is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_DIV


#if _USE_SOFT_F32_NEG == SIGMATEK_IMPL
float OPTIMIZED __negsf2(float a)
{
	float32 fa;
	fa.f = a;
	fa.parts.sign = !fa.parts.sign;
	return fa.f;
}
#elif _USE_SOFT_F32_NEG == LLVM_ASM_IMPL
float OPTIMIZED __negsf2(float a)
{
	asm volatile(
		"eor %[result], %[result]\n"
		: [result] "=r" (a) : : "cc");
	return a;
}
#elif _USE_SOFT_F32_NEG != NO_IMPL
#	error "_USE_SOFT_F32_NEG is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_NEG


#if _USE_SOFT_F64_NEG == SIGMATEK_IMPL
double OPTIMIZED __negdf2(double a)
{
	float64 fa;
	fa.d = a;
	fa.parts.sign = !fa.parts.sign;
	return fa.d;
}
#elif _USE_SOFT_F64_NEG == LLVM_ASM_IMPL
double OPTIMIZED __negdf2(double a)
{
	// HACK: maybe there's a better way to do this...
	asm volatile(
		"eor r1, r1\n"
		: : : "cc");
	return a;
}
#elif _USE_SOFT_F64_NEG != NO_IMPL
#	error "_USE_SOFT_F64_NEG is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_NEG


/* Conversion functions */

#if _USE_SOFT_F32_F64 == SIGMATEK_IMPL
double OPTIMIZED __extendsfdf2(float a)
{
	float32 fa;
	fa.f = a;
	return convertFloat32ToFloat64(fa).d;
}
#elif _USE_SOFT_F32_F64 == LLVM_ASM_IMPL
void NAKED __extendsfdf2(void)
{
	asm volatile(
		" .syntax unified\r\n"
		" vmov	s15, r0\r\n"
		" vcvt.f64.f32 d7, s15\r\n"
		" vmov	r0, r1, d7\r\n"
		" bx	lr\r\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F32_F64 != NO_IMPL
#	error "_USE_SOFT_F32_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_F64


#if _USE_SOFT_F64_F32 == SIGMATEK_IMPL
float OPTIMIZED __truncdfsf2(double a)
{
	float64 da;
	da.d = a;
	return convertFloat64ToFloat32(da).f;
}
#elif _USE_SOFT_F64_F32 == LLVM_ASM_IMPL
void NAKED __truncdfsf2(void)
{
	asm volatile(
		" .syntax unified\r\n"
		" vmov 	d7, r0, r1\r\n"
		" vcvt.f32.f64 s15, d7 \r\n"
		" vmov 	r0, s15  \r\n"
		" bx	lr \r\n"
		" .syntax divided\r\n"
		: : : "cc");
}
#elif _USE_SOFT_F64_F32 != NO_IMPL
#	error "_USE_SOFT_F64_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_F32


#if _USE_SOFT_F32_I32 == SIGMATEK_IMPL
si_int OPTIMIZED __fixsfsi(float a)
{
	float32 fa;
	fa.f = a;
		
	return float32_to_int32(fa);
}
#elif _USE_SOFT_F32_I32 == LLVM_ASM_IMPL
si_int OPTIMIZED __fixsfsi(float a)
{
	si_int r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.s32.f32 %[result], %[va]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a) : "cc");
	return r;
}
#elif _USE_SOFT_F32_I32 != NO_IMPL
#	error "_USE_SOFT_F32_I32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_I32


#if _USE_SOFT_F64_I32 == SIGMATEK_IMPL
si_int OPTIMIZED __fixdfsi(double a)
{
	float64 da;
	da.d = a;
		
	return float64_to_int32(da);
}
#elif _USE_SOFT_F64_I32 == LLVM_ASM_IMPL
si_int OPTIMIZED __fixdfsi(double a)
{
	si_int r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.s32.f64 %[result], %[va]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a) : "cc");
	return r;
}
#elif _USE_SOFT_F64_I32 != NO_IMPL
#	error "_USE_SOFT_F64_I32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_I32


// DS5303
#if _USE_SOFT_F32_I64 == SIGMATEK_IMPL
di_int OPTIMIZED __fixsfdi(float a)
{
	float32 fa;
	fa.f = a;
		
	return float32_to_int64(fa);
}
#elif _USE_SOFT_F32_I64 >= LLVM_C_IMPL
di_int OPTIMIZED __fixsfdi(float a)
{
	// Break a into sign, exponent, significand
	const rep_t aRep = toRep(a);
	const rep_t aAbs = aRep & absMask;
	const int sign = aRep & signBit ? -1 : 1;
	const int exponent = (aAbs >> significandBits) - exponentBias;
	const rep_t significand = (aAbs & significandMask) | implicitBit;

	// If 0 < exponent < significandBits, right shift to get the result.
	if ((unsigned int)exponent < significandBits)
	{
		return sign * (significand >> (significandBits - exponent));
	}
	// If exponent is negative, the result is zero.
	else if (exponent < 0)
	{
		return 0;
	}
	// If significandBits < exponent, left shift to get the result.  This shift
	// may end up being larger than the type width, which incurs undefined
	// behavior, but the conversion itself is undefined in that case, so
	// whatever the compiler decides to do is fine.
	else
	{
		return sign * (significand << (exponent - significandBits));
	}
}
#elif _USE_SOFT_F32_I64 != NO_IMPL
#	error "_USE_SOFT_F32_I64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_I64


// DS5303
#if _USE_SOFT_F64_I64 == SIGMATEK_IMPL
di_int OPTIMIZED __fixdfdi(double a)
{
	float64 da;
	da.d = a;
		
	return float64_to_int64(da);
}
#elif _USE_SOFT_F64_I64 >= LLVM_C_IMPL
di_int OPTIMIZED __fixdfdi(double a)
{
	double_bits fb;
	fb.f = a;
	int e = ((fb.u.s.high & 0x7FF00000) >> 20) - 1023;
	if (e < 0)
		return 0;
	di_int s = (si_int)(fb.u.s.high & 0x80000000) >> 31;
	dwords r;
	r.s.high = (fb.u.s.high & 0x000FFFFF) | 0x00100000;
	r.s.low = fb.u.s.low;
	if (e > 52)
		r.all <<= (e - 52);
	else
		r.all >>= (52 - e);
	return (r.all ^ s) - s;
}
#elif _USE_SOFT_F64_I64 != NO_IMPL
#	error "_USE_SOFT_F64_I64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_I64


#if _USE_SOFT_F32_I128 == SIGMATEK_IMPL
ti_int OPTIMIZED __fixsfti(float a)
{
	float32 fa;
	fa.f = a;
		
	return float32_to_int64(fa);
}
#elif _USE_SOFT_F32_I128 >= LLVM_C_IMPL
extern ti_int __fixsfti WEAKALIAS(__fixsfdi);
#elif _USE_SOFT_F32_I128 != NO_IMPL
#	error "_USE_SOFT_F32_I128 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_I128


#if _USE_SOFT_F64_I128 == SIGMATEK_IMPL
ti_int OPTIMIZED __fixdfti(double a)
{
	float64 da;
	da.d = a;
		
	return float64_to_int64(da);
}
#elif _USE_SOFT_F64_I128 >= LLVM_C_IMPL
extern ti_int __fixdfti WEAKALIAS(__fixdfdi);
#elif _USE_SOFT_F64_I128 != NO_IMPL
#	error "_USE_SOFT_F64_I128 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_I128


#if _USE_SOFT_F32_U32 == SIGMATEK_IMPL
su_int OPTIMIZED __fixunssfsi(float a)
{
	float32 fa;
	fa.f = a;
		
	return float32_to_uint32(fa);
}
#elif _USE_SOFT_F32_U32 == LLVM_ASM_IMPL
su_int OPTIMIZED __fixunssfsi(float a)
{
	su_int r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.u32.f32 %[result], %[va]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a) : "cc");
	return r;
}
#elif _USE_SOFT_F32_U32 != NO_IMPL
#	error "_USE_SOFT_F32_U32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_U32


#if _USE_SOFT_F64_U32 == SIGMATEK_IMPL
su_int OPTIMIZED __fixunsdfsi(double a)
{
	float64 da;
	da.d = a;
		
	return float64_to_uint32(da);
}
#elif _USE_SOFT_F64_U32 == LLVM_ASM_IMPL
su_int OPTIMIZED __fixunsdfsi(double a)
{
	su_int r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.u32.f64 %[result], %[va]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [va] "w" (a) : "cc");
	return r;
}
#elif _USE_SOFT_F64_U32 != NO_IMPL
#	error "_USE_SOFT_F64_U32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_U32


// DS5303
#if _USE_SOFT_F32_U64 == SIGMATEK_IMPL
du_int OPTIMIZED __fixunssfdi(float a)
{
	float32 fa;
	fa.f = a;
		
	return float32_to_uint64(fa);
}
#elif _USE_SOFT_F32_U64 >= LLVM_C_IMPL
du_int OPTIMIZED __fixunssfdi(float a)
{
	float_bits fb;
	fb.f = a;
	int e = ((fb.u & 0x7F800000) >> 23) - 127;
	if (e < 0 || (fb.u & 0x80000000))
		return 0;
	du_int r = (fb.u & 0x007FFFFF) | 0x00800000;
	if (e > 23)
		r <<= (e - 23);
	else
		r >>= (23 - e);
	return r;
}
#elif _USE_SOFT_F32_U64 != NO_IMPL
#	error "_USE_SOFT_F32_U64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_U64


// DS5303
#if _USE_SOFT_F64_U64 == SIGMATEK_IMPL
du_int OPTIMIZED __fixunsdfdi(double a)
{
	float64 da;
	da.d = a;
	// Grumat: This implementations seems to be broken and fails in the unit test
	return float64_to_uint64(da);
}
#elif _USE_SOFT_F64_U64 >= LLVM_C_IMPL
du_int OPTIMIZED __fixunsdfdi(double a)
{
	double_bits fb;
	fb.f = a;
	int e = ((fb.u.s.high & 0x7FF00000) >> 20) - 1023;
	if (e < 0 || (fb.u.s.high & 0x80000000))
		return 0;
	udwords r;
	r.s.high = (fb.u.s.high & 0x000FFFFF) | 0x00100000;
	r.s.low = fb.u.s.low;
	if (e > 52)
		r.all <<= (e - 52);
	else
		r.all >>= (52 - e);
	return r.all;
}
#elif _USE_SOFT_F64_U64 != NO_IMPL
#	error "_USE_SOFT_F64_U64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_U64


#if _USE_SOFT_F32_U128 == SIGMATEK_IMPL
tu_int OPTIMIZED __fixunssfti(float a)
{
	float32 fa;
	fa.f = a;

	return float32_to_uint64(fa);
}
#elif _USE_SOFT_F32_U128 >= LLVM_C_IMPL
extern ti_int __fixunssfti WEAKALIAS(__fixunssfdi);
#elif _USE_SOFT_F32_U128 != NO_IMPL
#	error "_USE_SOFT_F32_U128 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F32_U128


#if _USE_SOFT_F64_U128 == SIGMATEK_IMPL
tu_int OPTIMIZED __fixunsdfti(double a)
{
	float64 da;
	da.d = a;
	// Grumat: This implementations seems to be broken and fails in the unit test
	return float64_to_uint64(da);
}
#elif _USE_SOFT_F64_U128 >= LLVM_C_IMPL
extern ti_int __fixunsdfti WEAKALIAS(__fixunsdfdi);
#elif _USE_SOFT_F64_U128 != NO_IMPL
#	error "_USE_SOFT_F64_U128 is missing, invalid or unsupported"
#endif	// _USE_SOFT_F64_U128


#if _USE_SOFT_I32_F32 == SIGMATEK_IMPL
float OPTIMIZED __floatsisf(si_int i)
{
	float32 fa;
		
	fa = int32_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_I32_F32 == LLVM_ASM_IMPL
float OPTIMIZED __floatsisf(si_int i)
{
	float r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.f32.s32 %[result], %[vi]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [vi] "w" (i) : "cc");
	return r;
}
#elif _USE_SOFT_I32_F32 != NO_IMPL
#	error "_USE_SOFT_I32_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I32_F32


#if _USE_SOFT_I32_F64 == SIGMATEK_IMPL
double OPTIMIZED __floatsidf(si_int i)
{
	float64 da;
		
	da = int32_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_I32_F64 == LLVM_ASM_IMPL
double OPTIMIZED __floatsidf(si_int i)
{
	double r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.f64.s32 %[result], %[vi]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [vi] "w" (i) : "cc");
	return r;
}
#elif _USE_SOFT_I32_F64 != NO_IMPL
#	error "_USE_SOFT_I32_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I32_F64


// DS5303
#if _USE_SOFT_I64_F32 == SIGMATEK_IMPL
float OPTIMIZED __floatdisf(di_int i)
{
	float32 fa;
		
	fa = int64_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_I64_F32 >= LLVM_C_IMPL
float OPTIMIZED __floatdisf(di_int a)
{
	if (a == 0)
		return 0.0F;
	const unsigned N = sizeof(di_int) * CHAR_BIT;
	const di_int s = a >> (N - 1);
	a = (a ^ s) - s;
	int sd = N - __builtin_clzll(a);  /* number of significant digits */
	int e = sd - 1;             /* exponent */
	if (sd > FLT_MANT_DIG)
	{
		/*  start:  0000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQxxxxxxxxxxxxxxxxxx
		*  finish: 000000000000000000000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQR
		*                                                12345678901234567890123456
		*  1 = msb 1 bit
		*  P = bit FLT_MANT_DIG-1 bits to the right of 1
		*  Q = bit FLT_MANT_DIG bits to the right of 1
		*  R = "or" of all bits to the right of Q
		*/
		switch (sd)
		{
		case FLT_MANT_DIG + 1:
			a <<= 1;
			break;
		case FLT_MANT_DIG + 2:
			break;
		default:
			a = ((du_int)a >> (sd - (FLT_MANT_DIG + 2))) |
				((a & ((du_int)(-1) >> ((N + FLT_MANT_DIG + 2) - sd))) != 0);
		};
		/* finish: */
		a |= (a & 4) != 0;  /* Or P into R */
		++a;  /* round - this step may add a significant bit */
		a >>= 2;  /* dump Q and R */
				  /* a is now rounded to FLT_MANT_DIG or FLT_MANT_DIG+1 bits */
		if (a & ((du_int)1 << FLT_MANT_DIG))
		{
			a >>= 1;
			++e;
		}
		/* a is now rounded to FLT_MANT_DIG bits */
	}
	else
	{
		a <<= (FLT_MANT_DIG - sd);
		/* a is now rounded to FLT_MANT_DIG bits */
	}
	float_bits fb;
	fb.u = ((su_int)s & 0x80000000) |  /* sign */
		((e + 127) << 23) |  /* exponent */
		((su_int)a & 0x007FFFFF);   /* mantissa */
	return fb.f;
}
#elif _USE_SOFT_I64_F32 != NO_IMPL
#	error "_USE_SOFT_I64_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I64_F32


// DS5303
#if _USE_SOFT_I64_F64 == SIGMATEK_IMPL
double OPTIMIZED __floatdidf(di_int i)
{
	float64 da;
		
	da = int64_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_I64_F64 >= LLVM_C_IMPL
double OPTIMIZED __floatdidf(di_int a)
{
	static const double twop52 = 0x1.0p52;
	static const double twop32 = 0x1.0p32;

	union { double d; int64_t x; } low = { twop52 };

	const double high = (int32_t)(a >> 32) * twop32;
	low.x |= a & INT64_C(0x00000000ffffffff);

	const double result = (high - twop52) + low.d;
	return result;
}
#elif _USE_SOFT_I64_F64 != NO_IMPL
#	error "_USE_SOFT_I64_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I64_F64


#if _USE_SOFT_I128_F32 == SIGMATEK_IMPL
float OPTIMIZED __floattisf(ti_int i)
{
	float32 fa;
		
	fa = int64_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_I128_F32 >= LLVM_C_IMPL
extern ti_int __floattisf WEAKALIAS(__floatdisf);
#elif _USE_SOFT_I128_F32 != NO_IMPL
#	error "_USE_SOFT_I128_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I128_F32


#if _USE_SOFT_I128_F64 == SIGMATEK_IMPL
double OPTIMIZED __floattidf(ti_int i)
{
	float64 da;
		
	da = int64_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_I128_F64 >= LLVM_C_IMPL
extern ti_int __floattidf WEAKALIAS(__floatdidf);
#elif _USE_SOFT_I128_F64 != NO_IMPL
#	error "_USE_SOFT_I128_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_I128_F64


#if _USE_SOFT_U32_F32 == SIGMATEK_IMPL
float OPTIMIZED __floatunsisf(su_int i)
{
	float32 fa;
		
	fa = uint32_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_U32_F32 == LLVM_ASM_IMPL
float OPTIMIZED __floatunsisf(su_int i)
{
	float r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.f32.u32 %[result], %[vi]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [vi] "w" (i) : "cc");
	return r;
}
#elif _USE_SOFT_U32_F32 != NO_IMPL
#	error "_USE_SOFT_U32_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U32_F32


#if _USE_SOFT_U32_F64 == SIGMATEK_IMPL
double OPTIMIZED __floatunsidf(su_int i)
{
	float64 da;
		
	da = uint32_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_U32_F64 == LLVM_ASM_IMPL
double OPTIMIZED __floatunsidf(su_int i)
{
	double r;
	asm volatile(
		" .syntax unified\r\n"
		" vcvt.f64.u32 %[result], %[vi]\r\n"
		" .syntax divided\r\n"
		: [result] "=w" (r) : [vi] "w" (i) : "cc");
	return r;
}
#elif _USE_SOFT_U32_F64 != NO_IMPL
#	error "_USE_SOFT_U32_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U32_F64


// DS5303
#if _USE_SOFT_U64_F32 == SIGMATEK_IMPL
float OPTIMIZED __floatundisf(du_int i)
{
	float32 fa;
		
	fa = uint64_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_U64_F32 >= LLVM_C_IMPL
float OPTIMIZED __floatundisf(du_int a)
{
	if (a == 0)
		return 0.0F;
	const unsigned N = sizeof(du_int) * CHAR_BIT;
	int sd = N - __builtin_clzll(a);  /* number of significant digits */
	int e = sd - 1;             /* 8 exponent */
	if (sd > FLT_MANT_DIG)
	{
		/*  start:  0000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQxxxxxxxxxxxxxxxxxx
		*  finish: 000000000000000000000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQR
		*                                                12345678901234567890123456
		*  1 = msb 1 bit
		*  P = bit FLT_MANT_DIG-1 bits to the right of 1
		*  Q = bit FLT_MANT_DIG bits to the right of 1
		*  R = "or" of all bits to the right of Q
		*/
		switch (sd)
		{
		case FLT_MANT_DIG + 1:
			a <<= 1;
			break;
		case FLT_MANT_DIG + 2:
			break;
		default:
			a = (a >> (sd - (FLT_MANT_DIG + 2))) |
				((a & ((du_int)(-1) >> ((N + FLT_MANT_DIG + 2) - sd))) != 0);
		};
		/* finish: */
		a |= (a & 4) != 0;  /* Or P into R */
		++a;  /* round - this step may add a significant bit */
		a >>= 2;  /* dump Q and R */
				  /* a is now rounded to FLT_MANT_DIG or FLT_MANT_DIG+1 bits */
		if (a & ((du_int)1 << FLT_MANT_DIG))
		{
			a >>= 1;
			++e;
		}
		/* a is now rounded to FLT_MANT_DIG bits */
	}
	else
	{
		a <<= (FLT_MANT_DIG - sd);
		/* a is now rounded to FLT_MANT_DIG bits */
	}
	float_bits fb;
	fb.u = ((e + 127) << 23) |  /* exponent */
		((su_int)a & 0x007FFFFF);  /* mantissa */
	return fb.f;

}
#elif _USE_SOFT_U64_F32 != NO_IMPL
#	error "_USE_SOFT_U64_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U64_F32


// DS5303
#if _USE_SOFT_U64_F64 == SIGMATEK_IMPL
double OPTIMIZED __floatundidf(du_int i)
{
	float64 da;
		
	da = uint64_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_U64_F64 >= LLVM_C_IMPL
double OPTIMIZED __floatundidf(du_int a)
{
	static const double twop52 = 0x1.0p52;
	static const double twop84 = 0x1.0p84;
	static const double twop84_plus_twop52 = 0x1.00000001p84;

	union { double d; uint64_t x; } high = { twop84 };
	union { double d; uint64_t x; } low = { twop52 };

	high.x |= a >> 32;
	low.x |= a & UINT64_C(0x00000000ffffffff);

	const double result = (high.d - twop84_plus_twop52) + low.d;
	return result;
}
#elif _USE_SOFT_U64_F64 != NO_IMPL
#	error "_USE_SOFT_U64_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U64_F64


#if _USE_SOFT_U128_F32 == SIGMATEK_IMPL
float OPTIMIZED __floatuntisf(tu_int i)
{
	float32 fa;
		
	fa = uint64_to_float32(i);
	return fa.f;
}
#elif _USE_SOFT_U128_F32 >= LLVM_C_IMPL
extern ti_int __floatuntisf WEAKALIAS(__floatundisf);
#elif _USE_SOFT_U128_F32 != NO_IMPL
#	error "_USE_SOFT_U128_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U128_F32


#if _USE_SOFT_U128_F64 == SIGMATEK_IMPL
double OPTIMIZED __floatuntidf(tu_int i)
{
	float64 da;
		
	da = uint64_to_float64(i);
	return da.d;
}
#elif _USE_SOFT_U128_F64 >= LLVM_C_IMPL
extern ti_int __floatuntidf WEAKALIAS(__floatundidf);
#elif _USE_SOFT_U128_F64 != NO_IMPL
#	error "_USE_SOFT_U128_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_U128_F64

/* Comparison functions */
/* Comparison functions */

/* a<b .. -1
 * a=b ..  0
 * a>b ..  1
 * */

#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __cmpsf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				return 1; /* no special constant for unordered - maybe signaled? */
		};

		if (isFloat32eq(fa, fb)) {
				return 0;
		};
		
		if (isFloat32lt(fa, fb)) {
				return -1;
				};
		return 1;
}
#elif _USE_SOFT_CMP_F32 == LLVM_ASM_IMPL
int OPTIMIZED __cmpsf2(float a, float b)
{
	int r;
	asm volatile(
		" .syntax unified\r\n"
		" mov     r2, %[va], lsl #1\r\n"
		" mov     r3, %[vb], lsl #1\r\n"
		" orrs    r12, r2, r3, lsr #1\r\n"
		" eorsne  r12, %[va], %[vb]\r\n"
		" subspl  %[va], r2, r3\r\n"
		" mvnlo   %[result], r1, asr #31\r\n"
		" movhi   %[result], r1, asr #31\r\n"
		" orrne	%[result], %[result], #1\r\n"
		" cmp     r2, #0xff000000\r\n"
		" cmpls   r3, #0xff000000\r\n"
		" movhi   %[result], #1\r\n"
		" .syntax divided\r\n"
		: [result] "=r" (r) : [va] "r" (a), [vb] "r" (b) : "cc", "r2", "r3", "r12");
	return r;
}
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __cmpdf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				return 1; /* no special constant for unordered - maybe signaled? */
		};

		if (isFloat64eq(fa, fb)) {
				return 0;
		};
		
		if (isFloat64lt(fa, fb)) {
				return -1;
				};
		return 1;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
enum LE_RESULT
{
	LE_LESS = -1,
	LE_EQUAL = 0,
	LE_GREATER = 1,
	LE_UNORDERED = 1
};

int OPTIMIZED __cmpdf2(double a, double b)
{
	const srep_t aInt = toRep(a);
	const srep_t bInt = toRep(b);
	const rep_t aAbs = aInt & absMask;
	const rep_t bAbs = bInt & absMask;

	// If either a or b is NaN, they are unordered.
	if (aAbs > infRep || bAbs > infRep)
		return LE_UNORDERED;

	// If a and b are both zeros, they are equal.
	if ((aAbs | bAbs) == 0)
		return LE_EQUAL;

	// If at least one of a and b is positive, we get the same result comparing
	// a and b as signed integers as we would with a floating-point compare.
	if ((aInt & bInt) >= 0)
	{
		if (aInt < bInt)
			return LE_LESS;
		else if (aInt == bInt)
			return LE_EQUAL;
		else
			return LE_GREATER;
	}

	// Otherwise, both are negative, so we need to flip the sense of the
	// comparison to get the correct result.  (This assumes a twos- or ones-
	// complement integer representation; if integers are represented in a
	// sign-magnitude representation, then this flip is incorrect).
	else
	{
		if (aInt > bInt)
			return LE_LESS;
		else if (aInt == bInt)
			return LE_EQUAL;
		else
			return LE_GREATER;
	}
}
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __unordsf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		return ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) );
}
#elif _USE_SOFT_CMP_F32 == LLVM_ASM_IMPL
int OPTIMIZED __unordsf2(float a, float b)
{
	int r;
	asm volatile(
		" .syntax unified \r\n"
		" mov     r2, %[va], lsl #1 \r\n"
		" mov     r3, %[vb], lsl #1 \r\n"
		" mov     %[result], #0 \r\n"
		" cmp     r2, #0xff000000 \r\n"
		" cmpls   r3, #0xff000000 \r\n"
		" movhi   %[result], #1 \r\n"
		" .syntax divided \r\n"
		: [result] "=r" (r) : [va] "r" (a), [vb] "r" (b) : "cc", "r2", "r3");
	return r;
}
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __unorddf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		return ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) );
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
int OPTIMIZED __unorddf2(double a, double b)
{
	const rep_t aAbs = toRep(a) & absMask;
	const rep_t bAbs = toRep(b) & absMask;
	return aAbs > infRep || bAbs > infRep;
}
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


// float a == b
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __eqsf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		return isFloat32eq(fa, fb) - 1;
}
#elif _USE_SOFT_CMP_F32 >= LLVM_C_IMPL
// float a == b
extern int __eqsf2 WEAKALIAS(__cmpsf2);
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


// double a == b
#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __eqdf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		return isFloat64eq(fa, fb) - 1;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
// double a == b
extern int __eqdf2 WEAKALIAS(__cmpdf2);
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


// float a != b
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
extern int __nesf2 ALIAS(__eqsf2);
#elif _USE_SOFT_CMP_F32 >= LLVM_C_IMPL
// float a != b
extern int __nesf2 WEAKALIAS(__cmpsf2);
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


// double a != b
#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
extern int __nedf2 ALIAS(__eqdf2);
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
// double a != b
extern int __nedf2 WEAKALIAS(__cmpdf2);
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


/* return value >= 0 if a>=b and neither is NaN */
// float a >= b
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __gesf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return -1;
				};
		
		if (isFloat32eq(fa, fb))
		  return 0;

		if (isFloat32gt(fa, fb))
		  return 1;
		
		return -1;
}
#elif _USE_SOFT_CMP_F32 == LLVM_ASM_IMPL
/* return value >= 0 if a>=b and neither is NaN */
// float a >= b
int OPTIMIZED __gesf2(float a, float b)
{
	int r;
	asm volatile(
		" .syntax unified \r\n"
		" mov     r2, %[va], lsl #1			\r\n"
		" mov     r3, %[vb], lsl #1			\r\n"
		" orrs    r12, r2, r3, lsr #1		\r\n"
		" eorsne  r12, %[va], %[vb]			\r\n"
		" subspl  %[va], r2, r3				\r\n"
		" mvnlo   %[res], %[vb], asr #31	\r\n"
		" movhi   %[res], %[vb], asr #31	\r\n"
		" orrne	%[res], %[res], #1			\r\n"
		" cmp     r2, #0xff000000			\r\n"
		" cmpls   r3, #0xff000000			\r\n"
		" movhi   %[res], #-1				\r\n"
		" .syntax divided					\r\n"
		: [res] "=r" (r) : [va] "r" (a), [vb] "r" (b) : "cc", "r2", "r3", "r12");
	return r;
}
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __gedf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return -1;
				};
		
		if (isFloat64eq(fa, fb))
		  return 0;

		if (isFloat64gt(fa, fb))
		  return 1;
		
		return -1;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
enum GE_RESULT {
	GE_LESS = -1,
	GE_EQUAL = 0,
	GE_GREATER = 1,
	GE_UNORDERED = -1   // Note: different from LE_UNORDERED
};

int OPTIMIZED __gedf2(double a, double b)
{
	const srep_t aInt = toRep(a);
	const srep_t bInt = toRep(b);
	const rep_t aAbs = aInt & absMask;
	const rep_t bAbs = bInt & absMask;

	if (aAbs > infRep || bAbs > infRep)
		return GE_UNORDERED;
	if ((aAbs | bAbs) == 0)
		return GE_EQUAL;
	if ((aInt & bInt) >= 0)
	{
		if (aInt < bInt)
			return GE_LESS;
		else if (aInt == bInt)
			return GE_EQUAL;
		else
			return GE_GREATER;
	}
	else
	{
		if (aInt > bInt)
			return GE_LESS;
		else if (aInt == bInt)
			return GE_EQUAL;
		else
			return GE_GREATER;
	}
}
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


// float a < b
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __ltsf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		if (isFloat32lt(fa, fb)) {
				return -1;
				};
		return 0;
}
#elif _USE_SOFT_CMP_F32 >= LLVM_C_IMPL
// float a < b
extern int __ltsf2 WEAKALIAS(__cmpsf2);
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __ltdf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		if (isFloat64lt(fa, fb)) {
				return -1;
				};
		return 0;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
// double a < b
extern int __ltdf2 WEAKALIAS(__cmpdf2);
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


/* return value <= 0 if a<=b and neither is NaN */
#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __lesf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		
		if (isFloat32eq(fa, fb)) {
				return 0;
		};
		
		if (isFloat32lt(fa, fb)) {
				return -1;
				};
		
		return 1;
}
#elif _USE_SOFT_CMP_F32 >= LLVM_C_IMPL
/* return value <= 0 if a<=b and neither is NaN */
extern int __lesf2 WEAKALIAS(__cmpsf2);
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


/* return value <= 0 if a<=b and neither is NaN */
#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __ledf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return 1;
				};
		
		if (isFloat64eq(fa, fb)) {
				return 0;
		};
		
		if (isFloat64lt(fa, fb)) {
				return -1;
				};
		
		return 1;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
/* return value <= 0 if a<=b and neither is NaN */
extern int __ledf2 WEAKALIAS(__cmpdf2);
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


#if _USE_SOFT_CMP_F32 == SIGMATEK_IMPL
int OPTIMIZED __gtsf2(float a, float b)
{
		float32 fa, fb;
		fa.f = a;
		fb.f = b;
		if ( (isFloat32NaN(fa)) || (isFloat32NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return -1;
				};
		if (isFloat32gt(fa, fb)) {
				return 1;
				};
		return 0;
}
#elif _USE_SOFT_CMP_F32 >= LLVM_C_IMPL
extern int __gtsf2 WEAKALIAS(__gesf2);
#elif _USE_SOFT_CMP_F32 != NO_IMPL
#	error "_USE_SOFT_CMP_F32 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F32


#if _USE_SOFT_CMP_F64 == SIGMATEK_IMPL
int OPTIMIZED __gtdf2(double a, double b)
{
		float64 fa, fb;
		fa.d = a;
		fb.d = b;
		if ( (isFloat64NaN(fa)) || (isFloat64NaN(fb)) ) {
				/* TODO: sigNaNs*/
				return -1;
				};
		if (isFloat64gt(fa, fb)) {
				return 1;
				};
		return 0;
}
#elif _USE_SOFT_CMP_F64 >= LLVM_C_IMPL
extern int __gtdf2 WEAKALIAS(__gedf2);
#elif _USE_SOFT_CMP_F64 != NO_IMPL
#	error "_USE_SOFT_CMP_F64 is missing, invalid or unsupported"
#endif	// _USE_SOFT_CMP_F64


int OPTIMIZED WEAK __isnanf(float x) 
{
	return x != x;
}


int OPTIMIZED WEAK __isnand(double x)
{
	return x != x;
}





// ********************************************************************************************
// ST Funktionen ohne SIN, COS, ..
// ********************************************************************************************

#ifdef _USE_DIAS_R_FUNCTION

float OPTIMIZED DIAS_R_ABS(float a)
{
  if (a < 0)
	return -a;
  return a;
}

/*
*/
float OPTIMIZED DIAS_R_UTOR(unsigned int a)
{
  return (float) a;
}


/*
*/
float OPTIMIZED DIAS_R_DTOR(int a)
{
  return (float) a;
}



/*
*/
int OPTIMIZED DIAS_R_RTOD(float a)
{
  return (int) a;
}

/*
*/
float OPTIMIZED DIAS_R_SUB(float a, float b)
{
  return a - b;
}

/*
*/
float OPTIMIZED DIAS_R_ADD(float a, float b)
{
  return a + b;
}


/*
*/
float OPTIMIZED DIAS_R_MUL(float a, float b)
{
  return a * b;
}

/*
*/
float OPTIMIZED DIAS_R_DIV(float a, float b)
{
  return a / b;
}

/*
*/
float OPTIMIZED DIAS_R_OR(float a, float b)
{
  if (a || b)
	return 1.;
  return 0.;
}


/*
*/
float OPTIMIZED DIAS_R_XOR(float a, float b)
{
  if ((a == 0. && b != 0.) || (a != 0. && b == 0.))
	return 1.;
  return 0.;
}

float OPTIMIZED DIAS_R_AND(float a, float b)
{
  if (a && b)
	return 1.;
  return 0.;
}

/* Berechnet 1 / x

*/
float OPTIMIZED DIAS_R_1DX(float a)
{
  return 1 / a;
};

/*  Negiert den Wert
*/
float OPTIMIZED DIAS_R_NEG(float f)
{
  return -f;
}


/* Invertiert einen Float
	f != 0  --> 0.0
	f == 0  --> 1.0
*/
float OPTIMIZED DIAS_R_NOT(float f)
{
  if (f)
	return 0.;
  return 1.;
}

/* neue DIAS_R_CMP Funktion 

  unsigned long DIAS_R_CMP(float a, float b, uint32_t nBedType)
  nBedType: Bit0(1) : =
			Bit1(2) : <
			Bit2(4) : >
  Sample : (3.5, 7.6, 16#05) für >=
  Return: 0 wenn Bedingung NICHT erfüllt ist, 1 wenn Bedingung erfüllt ist
*/
uint32_t OPTIMIZED DIAS_R_CMP(float a, float b, uint32_t nBedType )
{
  uint32_t r = 0;

  if ((nBedType & 0x01) && a == b)
	r |= 1;
  if ((nBedType & 0x02) && a < b)
	r |= 1;
  if ((nBedType & 0x04) && a > b)
	r |= 1;
  return r;
}

#endif // _USE_DIAS_R_FUNCTION
#ifdef _USE_DIAS_L_FUNCTION

double OPTIMIZED DIAS_L_ABS(double a)
{
  if (a < 0)
	return -a;
  return a;
}

#ifdef _LSL_TARGETARCH_X86
long DIAS_L_RTOD(double a)
{
  long n;
  #ifdef _MSC_VER
	__asm fld QWORD PTR [EBP-8]
	__asm fistp DWORD PTR [EBP-12]
  #endif
  #ifdef _GNUC
	asm ("fldl %1 \n fistpl %0 " : "=m"(n) : "m"(a) : "memory" );
  #endif
  return n;
}
#endif

double OPTIMIZED DIAS_L_UTOR(unsigned int a)
{
  return (double) a;
}

double OPTIMIZED DIAS_L_DTOR(int a)
{
  return (double) a;
}

int OPTIMIZED DIAS_L_FTOD(double a)
{
  return (int) a;
}

double OPTIMIZED DIAS_L_SUB(double a, double b)
{
  return a - b;
}

double OPTIMIZED DIAS_L_ADD(double a, double b)
{
  return a + b;
}

double OPTIMIZED DIAS_L_MUL(double a, double b)
{
  return a * b;
}

double OPTIMIZED DIAS_L_DIV(double a, double b)
{
  return a / b;
}

double OPTIMIZED DIAS_L_OR(double a, double b)
{
  if (a || b)
	return 1.;
  return 0.;
}

double OPTIMIZED DIAS_L_XOR(double a, double b)
{
  if ((a == 0. && b != 0.) || (a != 0. && b == 0.))
	return 1.;
  return 0.;
}

double OPTIMIZED DIAS_L_AND(double a, double b)
{
  if (a && b)
	return 1.;
  return 0.;
}

double OPTIMIZED DIAS_L_1DX(double a)
{
  return 1 / a;
};

double OPTIMIZED DIAS_L_NEG(double f)
{
  return -f;
}

double OPTIMIZED DIAS_L_NOT(double f)
{
  if (f)
	return 0.;
  return 1.;
}


/* neue DIAS_L_CMP Funktion 

  unsigned long DIAS_R_CMP(double a, double b, uint32_t nBedType)
  nBedType: Bit0(1) : =
			Bit1(2) : <
			Bit2(4) : >
  Sample : (3.5, 7.6, 16#05) für >=
  Return: 0 wenn Bedingung NICHT erfüllt ist, 1 wenn Bedingung erfüllt ist
*/
unsigned long OPTIMIZED DIAS_L_CMP (double a, double b, unsigned long nBedType)
{

  unsigned long r = 0;

  if ((nBedType & 0x01))// && a == b)
   r = a == b;
  else if ((nBedType & 0x02))// && a < b)
   r = a < b;
  else if ((nBedType & 0x04))// && a > b)
   r =  a > b;
  else if ((nBedType & 0x08))// && a >= b)
   r = a >= b;
  else if ((nBedType & 0x10))// && a <= b)
   r = a <= b;
  else if ((nBedType & 0x20))// && a != b)
   r = a != b;
  else 
	r = 0;
	
  return r;

}

unsigned long OPTIMIZED DIAS_L_CLOSE()
{
  return 0;
}


#endif
// ********************************************************************************************
#ifdef __cplusplus
}
#endif
