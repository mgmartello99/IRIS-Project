#pragma once

#ifndef _MSC_VER

#define SAVE_FPU_STATE	1

typedef	unsigned int	bsd_fenv_t;
extern "C" bsd_fenv_t bsd_fegetround(void);
extern "C" void bsd_fesetround(bsd_fenv_t __round);

#if defined(__arm__)


#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)
#define	VFP_FE_TONEAREST	0x00000000
#define	VFP_FE_UPWARD		0x00400000
#define	VFP_FE_DOWNWARD		0x00800000
#define	VFP_FE_TOWARDZERO	0x00c00000
#define	FE_TONEAREST	VFP_FE_TONEAREST
#define	FE_UPWARD		VFP_FE_UPWARD
#define	FE_DOWNWARD		VFP_FE_DOWNWARD
#define	FE_TOWARDZERO	VFP_FE_TOWARDZERO


#define	vmrs_fpscr(__r)	__asm __volatile("fmrx %0, fpscr" : "=&r"(__r))
#define	vmsr_fpscr(__r)	__asm __volatile("fmxr fpscr, %0" : : "r"(__r))

#define __begin_mathbsd_fpu_context(ulvar1, ulvar2)	\
	__asm __volatile								\
	(												\
		"	.syntax divided \n"				\
		"	fmrx	%0, fpscr \n"				\
		"	bic		%1, %0, #12582912 \n"	\
		"	fmxr 	fpscr, %1 \n"				\
		: "=&r" (ulvar1), "=r" (ulvar2)	\
	)
#define __begin_mathbsd_fpu_context_single	__begin_mathbsd_fpu_context

#define __end_mathbsd_fpu_context(ulvar1)			\
	__asm __volatile								\
	(												\
		"	fmxr fpscr, %0 \n"					\
		:											\
		: "r" (ulvar1)						\
	)

#else	// !defined(__arm__)


#define	FE_TONEAREST	0x0000
#define	FE_DOWNWARD	0x0400
#define	FE_UPWARD	0x0800
#define	FE_TOWARDZERO	0x0c00
#define	_ROUND_MASK	(FE_TONEAREST | FE_DOWNWARD | \
			 FE_UPWARD | FE_TOWARDZERO)

#define	__fnstcw(__cw)		__asm __volatile("fnstcw %0" : "=m" (*(__cw)))
#define	__fldcw(__cw)		__asm __volatile("fldcw %0" : : "m" (__cw))

#define __begin_mathbsd_fpu_context(ulvar1, ulvar2)	\
	asm volatile									\
	(												\
		"	fnstcw	%[old] \n"						\
		"	movl	$0x127f, %[cur] \n"				\
		"	fldcw	%[cur] \n"						\
		: [old] "=m" (ulvar1), [cur] "=m" (ulvar2)	\
	)
#define __begin_mathbsd_fpu_context_single(ulvar1, ulvar2)	\
	asm volatile									\
	(												\
		"	fnstcw	%[old] \n"						\
		"	movl	$0x107f, %[cur] \n"				\
		"	fldcw	%[cur] \n"						\
		: [old] "=m" (ulvar1), [cur] "=m" (ulvar2)	\
	)

#define __end_mathbsd_fpu_context(ulvar1)				\
	asm volatile										\
	(													\
		"	fldcw	%[old] \n" : : [old] "m" (ulvar1)	\
	)

#endif	// defined(__arm__)

#else

#define SAVE_FPU_STATE	0

#endif	// _MSC_VER

