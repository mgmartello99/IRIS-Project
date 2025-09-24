#ifndef __FLOAT_EMU_CONFIG__201701031128__
#define __FLOAT_EMU_CONFIG__201701031128__


// Set this value to 0 to disable compiler optimizations and help debug
#define _OPTIMIZE_FLOAT_HELPER_EMU_	0


// ********************************************************************************************
// Definierung der Plattformabhängigen Funktionen
// ********************************************************************************************

#define NO_IMPL			0			// This value excludes implementation
#define SIGMATEK_IMPL	1			// Value for sigmatek soft math implementation
#define LLVM_C_IMPL		2			// Value for LLVM soft math C implementation
#define LLVM_ASM_IMPL	3			// Value for LLVM soft math ASM implementation (C if no ASM is available)
#define IFP_IMPL		4			// Emulates integer math using FP (for integers only)


#define LDIV_SOFT		1
#define LDIV_FPU		2
#define LDIV_ARM_ASM	3

// Controls Soft float implementation for ARM target
#ifdef _LSL_TARGETARCH_ARM
// Der ARM hat keine Divisions Cpu Befehl, deshalb braucht man dazu eine Funktion
#	define _USE_DIV_EMULATOR		LDIV_ARM_ASM
#	define _USE_MULDIV6432
#	define _USE_DIAS_R_FUNCTION		// Exportiere die DIAS_R_ Funktionen
#	define _USE_DIAS_L_FUNCTION		// Exportiere die DIAS_R_ Funktionen
#	define _USE_LLVM_SOFT_FLOAT		// Required for LLVM implementation

// Default implementation to use for soft floating point (if nowhere specified)
#	define _DEFAULT_SOFT_IMPL		NO_IMPL

#	define _USE_SOFT_F32_I64		LLVM_ASM_IMPL
#	define _USE_SOFT_F64_I64		LLVM_ASM_IMPL
#	define _USE_SOFT_F32_U64		LLVM_ASM_IMPL
#	define _USE_SOFT_F64_U64		LLVM_ASM_IMPL
#	define _USE_SOFT_I64_F32		LLVM_ASM_IMPL
#	define _USE_SOFT_I64_F64		LLVM_ASM_IMPL
#	define _USE_SOFT_U64_F32		LLVM_ASM_IMPL
#	define _USE_SOFT_U64_F64		LLVM_ASM_IMPL
#	define _USE_SOFT_I32_DIV		LLVM_ASM_IMPL
#	define _USE_SOFT_U32_DIV		LLVM_ASM_IMPL
#	define _USE_SOFT_I64_DIV		LLVM_C_IMPL
#	define _USE_SOFT_U64_DIV		LLVM_C_IMPL
#	define _USE_SOFT_I32_MOD		SIGMATEK_IMPL
#	define _USE_SOFT_U32_MOD		SIGMATEK_IMPL
#	define _USE_SOFT_I64_MOD		LLVM_C_IMPL
#	define _USE_SOFT_U64_MOD		LLVM_C_IMPL
#	define _USE_SOFT_I64_CMP		SIGMATEK_IMPL
#	define _USE_SOFT_U64_CMP		SIGMATEK_IMPL
#	define _USE_SOFT_U32_CLZ		LLVM_ASM_IMPL
#	define _USE_SOFT_U64_CLZ		LLVM_ASM_IMPL
#endif


// Controls Soft float implementation for x86 target
#ifdef _LSL_TARGETARCH_X86
#	define _USE_DIAS_L_FUNCTION

#	define _USE_STK_SOFT_FLOAT		// Required for Sigmatek implementation
#	define _USE_LLVM_SOFT_FLOAT		// Required for LLVM implementation

// Default implementation to use for soft floating point (if nowhere specified)
#	define _DEFAULT_SOFT_IMPL		NO_IMPL

#	define _USE_SOFT_F32_U64		LLVM_C_IMPL
#	define _USE_SOFT_F64_U64		LLVM_C_IMPL
#	define _USE_SOFT_I64_DIV		LLVM_C_IMPL
#	define _USE_SOFT_U64_DIV		LLVM_C_IMPL
#	define _USE_SOFT_I64_MOD		LLVM_C_IMPL
#	define _USE_SOFT_U64_MOD		LLVM_C_IMPL
#	define _USE_SOFT_I64_CMP		SIGMATEK_IMPL
#	define _USE_SOFT_U64_CMP		SIGMATEK_IMPL
#	define _USE_SOFT_U32_CLZ		LLVM_C_IMPL
#	define _USE_SOFT_U64_CLZ		LLVM_C_IMPL
#endif


// Sets default implementation values
// float1 + float2
#ifndef _USE_SOFT_F32_ADD
#	define _USE_SOFT_F32_ADD	_DEFAULT_SOFT_IMPL
#endif
// double1 + double2
#ifndef _USE_SOFT_F64_ADD
#	define _USE_SOFT_F64_ADD	_DEFAULT_SOFT_IMPL
#endif
// float1 - float2
#ifndef _USE_SOFT_F32_SUB
#	define _USE_SOFT_F32_SUB	_DEFAULT_SOFT_IMPL
#endif
// double1 - double2
#ifndef _USE_SOFT_F64_SUB
#	define _USE_SOFT_F64_SUB	_DEFAULT_SOFT_IMPL
#endif
// float1 * float2
#ifndef _USE_SOFT_F32_MUL
#	define _USE_SOFT_F32_MUL	_DEFAULT_SOFT_IMPL
#endif
// double1 * double2
#ifndef _USE_SOFT_F64_MUL
#	define _USE_SOFT_F64_MUL	_DEFAULT_SOFT_IMPL
#endif
// float1 / float2
#ifndef _USE_SOFT_F32_DIV
#	define _USE_SOFT_F32_DIV	_DEFAULT_SOFT_IMPL
#endif
// double1 / double2
#ifndef _USE_SOFT_F64_DIV
#	define _USE_SOFT_F64_DIV	_DEFAULT_SOFT_IMPL
#endif
// long1 / long2	(__divsi3)
#ifndef _USE_SOFT_I32_DIV
#	define _USE_SOFT_I32_DIV	_DEFAULT_SOFT_IMPL
#endif
// ulong1 / ulong2	(__udivsi3)
#ifndef _USE_SOFT_U32_DIV
#	define _USE_SOFT_U32_DIV	_DEFAULT_SOFT_IMPL
#endif
// int64_1 / int64_2	(__divdi3)
#ifndef _USE_SOFT_I64_DIV
#	define _USE_SOFT_I64_DIV	_DEFAULT_SOFT_IMPL
#endif
// uint64_1 / uint64_2	(__udivdi3)
#ifndef _USE_SOFT_U64_DIV
#	define _USE_SOFT_U64_DIV	_DEFAULT_SOFT_IMPL
#endif

// long1 % long2	(__modsi3)
#ifndef _USE_SOFT_I32_MOD
#	define _USE_SOFT_I32_MOD	_DEFAULT_SOFT_IMPL
#endif
// ulong1 % ulong2	(__umodsi3)
#ifndef _USE_SOFT_U32_MOD
#	define _USE_SOFT_U32_MOD	_DEFAULT_SOFT_IMPL
#endif
// int64_1 % int64_2	(__moddi3)
#ifndef _USE_SOFT_I64_MOD
#	define _USE_SOFT_I64_MOD	_DEFAULT_SOFT_IMPL
#endif
// uint64_1 % uint64_2	(__umoddi3)
#ifndef _USE_SOFT_U64_MOD
#	define _USE_SOFT_U64_MOD	_DEFAULT_SOFT_IMPL
#endif

// int64_1 == int64_2	(__cmpdi2)
#ifndef _USE_SOFT_I64_CMP
#	define _USE_SOFT_I64_CMP	_DEFAULT_SOFT_IMPL
#endif
// uint64_1 == uint64_2	(__ucmpdi2)
#ifndef _USE_SOFT_U64_CMP
#	define _USE_SOFT_U64_CMP	_DEFAULT_SOFT_IMPL
#endif

// clz(ulong) / __builtin_clzl(ulong)
#ifndef _USE_SOFT_U32_CLZ
#	define _USE_SOFT_U32_CLZ	_DEFAULT_SOFT_IMPL
#endif
// clz(uint64) / __builtin_clzll(uint64)
#ifndef _USE_SOFT_U64_CLZ
#	define _USE_SOFT_U64_CLZ	_DEFAULT_SOFT_IMPL
#endif

// -float1
#ifndef _USE_SOFT_F32_NEG
#	define _USE_SOFT_F32_NEG	_DEFAULT_SOFT_IMPL
#endif
// -double1
#ifndef _USE_SOFT_F64_NEG
#	define _USE_SOFT_F64_NEG	_DEFAULT_SOFT_IMPL
#endif

// (double)float1
#ifndef _USE_SOFT_F32_F64
#	define _USE_SOFT_F32_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)double1
#ifndef _USE_SOFT_F64_F32
#	define _USE_SOFT_F64_F32	_DEFAULT_SOFT_IMPL
#endif
// (long)float1
#ifndef _USE_SOFT_F32_I32
#	define _USE_SOFT_F32_I32	_DEFAULT_SOFT_IMPL
#endif
// (long)double1
#ifndef _USE_SOFT_F64_I32
#	define _USE_SOFT_F64_I32	_DEFAULT_SOFT_IMPL
#endif
// (long long)float1
#ifndef _USE_SOFT_F32_I64
#	define _USE_SOFT_F32_I64	_DEFAULT_SOFT_IMPL
#endif
// (long long)double1
#ifndef _USE_SOFT_F64_I64
#	define _USE_SOFT_F64_I64	_DEFAULT_SOFT_IMPL
#endif
// (int128_t)float1
#ifndef _USE_SOFT_F32_I128
#	define _USE_SOFT_F32_I128	_DEFAULT_SOFT_IMPL
#endif
// (int128_t)double1
#ifndef _USE_SOFT_F64_I128
#	define _USE_SOFT_F64_I128	_DEFAULT_SOFT_IMPL
#endif
// (unsigned long)float1
#ifndef _USE_SOFT_F32_U32
#	define _USE_SOFT_F32_U32	_DEFAULT_SOFT_IMPL
#endif
// (unsigned long)double1
#ifndef _USE_SOFT_F64_U32
#	define _USE_SOFT_F64_U32	_DEFAULT_SOFT_IMPL
#endif
// (unsigned long long)float1
#ifndef _USE_SOFT_F32_U64
#	define _USE_SOFT_F32_U64	_DEFAULT_SOFT_IMPL
#endif
// (unsigned long long)double1
#ifndef _USE_SOFT_F64_U64
#	define _USE_SOFT_F64_U64	_DEFAULT_SOFT_IMPL
#endif
// (unsigned int128_t)float1
#ifndef _USE_SOFT_F32_U128
#	define _USE_SOFT_F32_U128	_DEFAULT_SOFT_IMPL
#endif
// (unsigned int128_t)double1
#ifndef _USE_SOFT_F64_U128
#	define _USE_SOFT_F64_U128	_DEFAULT_SOFT_IMPL
#endif
// (float)long1
#ifndef _USE_SOFT_I32_F32
#	define _USE_SOFT_I32_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)long1
#ifndef _USE_SOFT_I32_F64
#	define _USE_SOFT_I32_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)i64
#ifndef _USE_SOFT_I64_F32
#	define _USE_SOFT_I64_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)i64
#ifndef _USE_SOFT_I64_F64
#	define _USE_SOFT_I64_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)i128
#ifndef _USE_SOFT_I128_F32
#	define _USE_SOFT_I128_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)i128
#ifndef _USE_SOFT_I128_F64
#	define _USE_SOFT_I128_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)ulong1
#ifndef _USE_SOFT_U32_F32
#	define _USE_SOFT_U32_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)ulong1
#ifndef _USE_SOFT_U32_F64
#	define _USE_SOFT_U32_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)ui64
#ifndef _USE_SOFT_U64_F32
#	define _USE_SOFT_U64_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)ui64
#ifndef _USE_SOFT_U64_F64
#	define _USE_SOFT_U64_F64	_DEFAULT_SOFT_IMPL
#endif
// (float)ui128
#ifndef _USE_SOFT_U128_F32
#	define _USE_SOFT_U128_F32	_DEFAULT_SOFT_IMPL
#endif
// (double)ui128
#ifndef _USE_SOFT_U128_F64
#	define _USE_SOFT_U128_F64	_DEFAULT_SOFT_IMPL
#endif
// float1 == float2
#ifndef _USE_SOFT_CMP_F32
#	define _USE_SOFT_CMP_F32	_DEFAULT_SOFT_IMPL
#endif
// double1 == double2
#ifndef _USE_SOFT_CMP_F64
#	define _USE_SOFT_CMP_F64	_DEFAULT_SOFT_IMPL
#endif


#endif	// __FLOAT_EMU_CONFIG__201701031128__
