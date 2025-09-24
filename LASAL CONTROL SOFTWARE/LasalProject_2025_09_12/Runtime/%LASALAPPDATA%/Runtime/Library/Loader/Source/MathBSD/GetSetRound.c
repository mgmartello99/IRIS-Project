#include "GetSetRound.h"

#ifndef _MSC_VER

#if defined(__arm__)


typedef	unsigned int	bsd_fenv_t;

bsd_fenv_t bsd_fegetround(void)
{
	bsd_fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	return (__fpsr & _ROUND_MASK);
}

void bsd_fesetround(bsd_fenv_t __round)
{
	bsd_fenv_t __fpsr;

	vmrs_fpscr(__fpsr);
	__fpsr &= ~(_ROUND_MASK);
	__fpsr |= __round;
	vmsr_fpscr(__fpsr);
}


#else	// defined(__arm__)


bsd_fenv_t bsd_fegetround(void)
{
	unsigned short __control;

	/*
	 * We assume that the x87 and the SSE unit agree on the
	 * rounding mode.  Reading the control word on the x87 turns
	 * out to be about 5 times faster than reading it on the SSE
	 * unit on an Opteron 244.
	 */
	__fnstcw(&__control);
	return (__control & _ROUND_MASK);
}


void bsd_fesetround(bsd_fenv_t __round)
{
	unsigned short __control;

	__fnstcw(&__control);
	__control &= ~_ROUND_MASK;
	__control |= __round;
	__fldcw(__control);

}


#endif

#endif	// _MSC_VER
