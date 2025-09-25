/* $$!FNOTE 10 */
/****************************************************************************/
/*                                                                          */
/*  File: SYSCRT.H                                                          */
/*  Date: April 15, 2000                                                    */
/*  By:                                                                     */
/*  Description:                                                            */
/*		This file contains the SYSCRT C Runtime Library Interface.    		*/
/*  Revision History:                                                       */
/*      Rev     By      Date        Description                             */
/*      ---     --      ----        -----------                             */
/*      1.00    THR     4/15/00     Initial Version                         */
/*                                                                          */
/****************************************************************************/
/* $$!ENDCMD */
//
//	The SYSCRT provides an Interface Wrapper for various C RUntime functions.
//
//	The interface is set as unlimited and can be accessed as required.
//
#ifndef syscrt_h
#pragma once
#define syscrt_h

//
//	SYSCRT Interface Structure
//
//	The following structure is the SYSCRT Interface Structure.  The structure simply
//	wraps various C Runtime functions.
//
typedef struct
{
	// Math Functions
	long (* ftol)(double);
	int     (* abs)(int);
	double  (* acos)(double);
	double  (* asin)(double);
	double  (* atan)(double);
	double  (* atan2)(double, double);
	double  (* cos)(double);
	double  (* cosh)(double);
	double  (* exp)(double);
	double  (* fabs)(double);
	double  (* fmod)(double, double);
	long    (* labs)(long);
	double  (* log)(double);
	double  (* log10)(double);
	double  (* pow)(double, double);
	double  (* sin)(double);
	double  (* sinh)(double);
	double  (* tan)(double);
	double  (* tanh)(double);
	double  (* sqrt)(double);
	double  (* atof)(const char *);
	double  (* ceil)(double);
	double  (* floor)(double);
	double  (* frexp)(double, int *);
	double  (* _hypot)(double, double);
	double  (* _j0)(double);
	double  (* _j1)(double);
	double  (* _jn)(int, double);
	double  (* ldexp)(double, int);
	double  (* modf)(double, double *);
	double  (* _y0)(double);
	double  (* _y1)(double);
	double  (* _yn)(int, double);
	int (* sprintf)(char *s, const char *format, ... );

} SYS_ICRT;

//
//	Initialization Function
//	(not callable by application)
//
// SYS_ERROR Sys_CRTInit();


//
//	Macros for use by C Applications
//
#define FTOL(dbl)(LSL_POS->piCRT->ftol)(dbl)
#define ABS(intval)(LSL_POS->piCRT->abs)(intval)
#define ACOS(dbl)(LSL_POS->piCRT->acos)(dbl)
#define ASIN(dbl)(LSL_POS->piCRT->asin)(dbl)
#define ATAN(dbl)(LSL_POS->piCRT->atan)(dbl)
#define ATAN2(dbl1,dbl2)(LSL_POS->piCRT->atan2)(dbl1, dbl2)
#define COS(dbl)(LSL_POS->piCRT->cos)(dbl)
#define COSH(dbl)(LSL_POS->piCRT->cosh)(dbl)
#define EXP(dbl)(LSL_POS->piCRT->exp)(dbl)
#define FABS(dbl)(LSL_POS->piCRT->fabs)(dbl)
#define FMOD(dbl1,dbl2)(LSL_POS->piCRT->fmod)(dbl1,dbl2)
#define LABS(lngval)(LSL_POS->piCRT->labs)(lngval)
#define LOG(dbl)(LSL_POS->piCRT->log)(dbl)
#define LOG10(dbl)(LSL_POS->piCRT->log10)(dbl)
#define POW(dbl1,dbl2)(LSL_POS->piCRT->pow)(dbl1,dbl2)
#define SIN(dbl)(LSL_POS->piCRT->sin)(dbl)
#define SINH(dbl)(LSL_POS->piCRT->sinh)(dbl)
#define TAN(dbl)(LSL_POS->piCRT->tan)(dbl)
#define TANH(dbl)(LSL_POS->piCRT->tanh)(dbl)
#define SQRT(dbl)(LSL_POS->piCRT->sqrt)(dbl)
#define ATOF(str)(LSL_POS->piCRT->atof)(str)
#define CEIL(dbl)(LSL_POS->piCRT->ceil)(dbl)
#define FLOOR(dbl)(LSL_POS->piCRT->floor)(dbl)
#define FREXP(dbl,pint)(LSL_POS->piCRT->frexp)(dbl,pint)
#define _HYPOT(dbl1,dbl2)(LSL_POS->piCRT->_hypot)(dbl1,dbl2)
#define _J0(dbl)(LSL_POS->piCRT->_j0)(dbl)
#define _J1(dbl)(LSL_POS->piCRT->_j1)(dbl)
#define _JN(intval,dbl)(LSL_POS->piCRT->_jn)(intval,dbl)
#define LDEXP(dbl,intval)(LSL_POS->piCRT->ldexp)(dbl,intval)
#define MODF(dbl,pdbl)(LSL_POS->piCRT->modf)(dbl,pdbl)
#define _Y0(dbl)(LSL_POS->piCRT->_y0)(dbl)
#define _Y1(dbl)(LSL_POS->piCRT->_y1)(dbl)
#define _YN(intval,dbl)(LSL_POS->piCRT->_yn)(intval,dbl)

#endif // syscrt_h




