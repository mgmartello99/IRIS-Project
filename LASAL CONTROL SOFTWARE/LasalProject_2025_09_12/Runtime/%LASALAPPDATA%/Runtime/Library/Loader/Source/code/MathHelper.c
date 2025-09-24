/********************************************************************************************
Floating Point Emulationssourcen
********************************************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

// ********************************************************************************************
// Definierung der Plattformabhängigen Funktionen
// ********************************************************************************************

#ifdef _LSL_TARGETARCH_ARM
  #define _USE_DIAS_R_FUNCTION  // Exportiere die DIAS_R_ Funktionen
#endif

// ********************************************************************************************
// Diverse Prototypen
// ********************************************************************************************

int ispos (double x);
int isposf (float x);
double exp(double x);
float expf(float x);
double fabs(double x);
float fabsf(float x);
int isnan(double x);
int isnanf(float x);
double frexp (double d, int *exp);
float frexpf (float d, int *exp);
float logarithmf(float x, int ten);
float logf(float x);
double logarithm(double x, int ten);
double log(double x);
int numtest(double x);
int numtestf(float x);
double ldexp(double d, int e);
float ldexpf(float d, int e);
float modff(float x, float *iptr);
double modf(double x, double *iptr);
float powf(float x, float y);
double powd(double x, double y);
double sine(double x, int cosine);
double sin(double x);
float sinef(float x, int cosine);
float sinf(float x);
double cos(double x);
float cosf(float x);
double tan(double x);
double asine(double x, int acosine);
double asin(double x);
double acos(double x);
double atangent(double x, double v, double u, int arctan2);
double atan(double x);
float asinef(float x, int acosine);
float asinf(float x);
float acosf(float x);
float atangentf(float x, float v, float u, int arctan2);
float atanf(float x);
double sqrt(double x);
float sqrtf(float x);

// ********************************************************************************************
// Typdefinitionen , Strukturen, Konstanten
// ********************************************************************************************

typedef unsigned int __uint32_t;
typedef int __int32_t;

typedef union
{
  float value;
  __uint32_t word;
} ieee_float_shape_type;

typedef union 
{
  double value;
  struct 
  {
    __uint32_t lsw;
    __uint32_t msw;
  } parts;
} ieee_double_shape_type;


typedef const union
{
  long l[2];
  double d;
} udouble;

typedef const union
{
  long l;
  float f;
} ufloat;

#define DOUBLE_EXP_OFFS 1023
#define DBL_MAX_EXP     1024                    /* max binary exponent */
#define DBL_MIN_EXP     (-1021)                 /* min binary exponent */
#define DBL_MAX         1.7976931348623158e+308 /* max value */

#define FLOAT_EXP_OFFS 127
#define FLT_MAX_EXP     128                     /* max binary exponent */
#define FLT_MIN_EXP     (-125)                  /* min binary exponent */
#define FLT_MAX         3.402823466e+38F        /* max value */

#define NUM 3
#define NAN 2
#define INF 1 

static const double __PI            = 3.14159265358979323846;
static double __SQRT_HALF     = 0.70710678118654752440;
static double __PI_OVER_TWO   = 1.57079632679489661923132;
static float  FLT_PI          = 3.14159265358979323846;

#define FLT_UWORD_IS_NAN(x) ((x)>0x7f800000L) 

static double BIGX = 7.09782712893383973096e+02;
static double SMALLX = -7.45133219101941108420e+02; 
static double z_rooteps = 7.4505859692e-9;
static float  z_rooteps_f = 1.7263349182589107e-4;
 
static ufloat z_infinity_f = { 0x7f800000 };
static ufloat z_notanum_f  = { 0xffd00000 };
static udouble z_infinity = { { 0, 0x7ff00000 } };
static udouble z_notanum  = { { 0, 0xfff80000 } }; 

static const float oneF = 1.0;
static const double oneD = 1.0;

enum EMathError { EMathNo, EDOM, ERANGE };
static int errno = 0;

/* Get the more significant 32 bit int from a double.  */
#define GET_HIGH_WORD(i,d)					\
do {								\
  ieee_double_shape_type gh_u;					\
  gh_u.value = (d);						\
  (i) = gh_u.parts.msw;						\
} while (0) 


/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,v)					\
do {								\
  ieee_double_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Get two 32 bit ints from a double.  */
#define EXTRACT_WORDS(ix0,ix1,d)				\
do {								\
  ieee_double_shape_type ew_u;					\
  ew_u.value = (d);						\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0) 

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,ix0,ix1)					\
do {								\
  ieee_double_shape_type iw_u;					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0) 


/* Get a 32 bit int from a float.  */ 
#define GET_FLOAT_WORD(i,d)					\
do {								\
  ieee_float_shape_type gf_u;					\
  gf_u.value = (d);						\
  (i) = gf_u.word;						\
} while (0) 

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d,i)					\
do {								\
  ieee_float_shape_type sf_u;					\
  sf_u.word = (i);						\
  (d) = sf_u.value;						\
} while (0) 

/************************************************************/
#define 	isfinitef(x)    ((x) >= -FLT_MAX && (x) <= FLT_MAX)
#define 	isfinite(x)     ((x) >= -DBL_MAX && (x) <= DBL_MAX)

/************************************************************/

// ********************************************************************************************
// Funktionen
// ********************************************************************************************

int ispos (double x)
{
  __uint32_t hx;

  GET_HIGH_WORD (hx, x);

  if (hx & 0x80000000)
    return (0);
  else
    return (1);
} 

int isposf (float x)
{
  __int32_t wx;

  GET_FLOAT_WORD (wx, x);

  if (wx & 0x80000000)
    return (0);
  else
    return (1);
} 

static const double exp_INV_LN2 = 1.4426950408889634074;
static const double exp_LN2 = 0.6931471805599453094172321;
static const double exp_p[] = { 0.25, 0.75753180159422776666e-2, 0.31555192765684646356e-4 };
static const double exp_q[] = { 0.5, 0.56817302698551221787e-1,  0.63121894374398504557e-3, 0.75104028399870046114e-6 };
 
double exp(double x)
{
  int N;
  double g, z, R, P, Q;

  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = ERANGE;
        if (ispos (x))
          return (z_infinity.d);
        else
          return (0.0);
      case 0:
        return (1.0);
    }

  /* Check for out of bounds. */
  if (x > BIGX || x < SMALLX)
    {
      errno = ERANGE;
      return (x);
    }

  /* Check for a value too small to calculate. */
  if (-z_rooteps < x && x < z_rooteps)
    {
      return (1.0);
    }

  /* Calculate the exponent. */
  if (x < 0.0)
    N = (int) (x * exp_INV_LN2 - 0.5);
  else
    N = (int) (x * exp_INV_LN2 + 0.5);

  /* Construct the mantissa. */
  g = x - N * exp_LN2;
  z = g * g;
  P = g * ((exp_p[2] * z + exp_p[1]) * z + exp_p[0]);
  Q = ((exp_q[3] * z + exp_q[2]) * z + exp_q[1]) * z + exp_q[0];
  R = 0.5 + P / (Q - P);

  /* Return the floating point value. */
  N++;
  return (ldexp (R, N));
}
 
static const float expf_INV_LN2 = 1.442695040;
static const float expf_LN2 = 0.693147180;
static const float expf_p[] = { 0.249999999950, 0.00416028863 };
static const float expf_q[] = { 0.5, 0.04998717878 };

float expf(float x)
{
  int N;
  float g, z, R, P, Q;

  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = ERANGE;
        if (isposf (x))
          return (z_infinity_f.f);
        else
          return (0.0);
      case 0:
        return (1.0);
    }

  /* Check for out of bounds. */
  if (x > BIGX || x < SMALLX)
    {
      errno = ERANGE;
      return (x);
    }

  /* Check for a value too small to calculate. */
  if (-z_rooteps_f < x && x < z_rooteps_f)
    {
      return (1.0);
    }

  /* Calculate the exponent. */
  if (x < 0.0)
    N = (int) (x * expf_INV_LN2 - 0.5);
  else
    N = (int) (x * expf_INV_LN2 + 0.5);

  /* Construct the mantissa. */
  g = x - N * expf_LN2;
  z = g * g;
  P = g * (expf_p[1] * z + expf_p[0]);
  Q = expf_q[1] * z + expf_q[0];
  R = 0.5 + P / (Q - P);

  /* Return the floating point value. */
  N++;
  return (ldexpf (R, N));
}
 
double fabs(double x)
{
  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = ERANGE;
        return (x);
      case 0:
        return (0.0);
      default:
        return (x < 0.0 ? -x : x);
    }
}
 
float fabsf(float x)
{
  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = ERANGE;
        return (x);
      case 0:
        return (0.0);
      default:
        return (x < 0.0 ? -x : x);
    }
}

int isnan(double x)
{
	__int32_t hx,lx;
	EXTRACT_WORDS(hx,lx,x);
	hx &= 0x7fffffff;
	hx |= (__uint32_t)(lx|(-lx))>>31;	
	hx = 0x7ff00000 - hx;
	return (int)(((__uint32_t)(hx))>>31);
} 

int isnanf(float x)
{
	__int32_t ix;
	GET_FLOAT_WORD(ix,x);
	ix &= 0x7fffffff;
	return FLT_UWORD_IS_NAN(ix);
}

double frexp (double d, int *exp)
{
  double f;
  __uint32_t hd, ld, hf, lf;

  /* Check for special values. */
  switch (numtest (d))
    {
      case NAN:
      case INF:
        errno = EDOM;
      case 0:
        *exp = 0;
        return (d);
    }

  EXTRACT_WORDS (hd, ld, d);

  /* Get the exponent. */
  *exp = ((hd & 0x7ff00000) >> 20) - 1022;

  /* Get the mantissa. */ 
  lf = ld;
  hf = hd & 0x800fffff;  
  hf |= 0x3fe00000;

  INSERT_WORDS (f, hf, lf);

  return (f);
}

float frexpf (float d, int *exp)
{
  float f;
  __int32_t wf, wd;

  /* Check for special values. */
  switch (numtestf (d))
    {
      case NAN:
      case INF:
        errno = EDOM;
      case 0:
        *exp = 0;
        return (d);
    }

  GET_FLOAT_WORD (wd, d);

  /* Get the exponent. */
  *exp = ((wd & 0x7f800000) >> 23) - 126;

  /* Get the mantissa. */ 
  wf = wd & 0x7fffff;  
  wf |= 0x3f000000;

  SET_FLOAT_WORD (f, wf);

  return (f);
}
 

static const float logarithmf_a[] = { -0.5527074855 };
static const float logarithmf_b[] = { -0.6632718214e+1 };
static const float logarithmf_C1 = 0.693145752;
static const float logarithmf_C2 = 1.428606820e-06;
static const float logarithmf_C3 = 0.4342944819;

float logarithmf(float x, int ten)
{
  int N;
  float f, w, z;

  /* Check for domain/range errors here. */
  if (x == 0.0)
    {
      errno = ERANGE;
      return (-z_infinity_f.f);
    }
  else if (x < 0.0)
    {
      errno = EDOM;
      return (z_notanum_f.f);
    }
  else if (!isfinitef(x))
    {
      if (isnanf(x)) 
        return (z_notanum_f.f);
      else
        return (z_infinity_f.f);
    }

  /* Get the exponent and mantissa where x = f * 2^N. */
  f = frexpf (x, &N);

  z = f - 0.5;

  if (f > __SQRT_HALF)
    z = (z - 0.5) / (f * 0.5 + 0.5);
  else
    {
      N--;
      z /= (z * 0.5 + 0.5);
    }
  w = z * z;

  /* Use Newton's method with 4 terms. */
  z += z * w * (logarithmf_a[0]) / ((w + 1.0) * w + logarithmf_b[0]);

  if (N != 0)
    z = (N * logarithmf_C2 + z) + N * logarithmf_C1;

  if (ten)
    z *= logarithmf_C3;

  return (z);
} 

float logf(float x)
{
  return (logarithmf (x, 0));
}

float log10f(float x)
{
  return (logarithmf (x, 1));
}

static const double logarithm_a[] = { -0.64124943423745581147e+02, 0.16383943563021534222e+02, -0.78956112887481257267 };
static const double logarithm_b[] = { -0.76949932108494879777e+03, 0.31203222091924532844e+03, -0.35667977739034646171e+02 };
static const double logarithm_C1 =  22713.0 / 32768.0;
static const double logarithm_C2 =  1.428606820309417232e-06;
static const double logarithm_C3 =  0.43429448190325182765;

double logarithm(double x, int ten)
{
  int N;
  double f, w, z;

  /* Check for range and domain errors here. */
  if (x == 0.0)
    {
      errno = ERANGE;
      return (-z_infinity.d);
    }
  else if (x < 0.0)
    {
      errno = EDOM;
      return (z_notanum.d);
    }
  else if (!isfinite(x))
    {
      if (isnan(x))
        return (z_notanum.d);
      else
        return (z_infinity.d);
    }

  /* Get the exponent and mantissa where x = f * 2^N. */
  f = frexp (x, &N);

  z = f - 0.5;

  if (f > __SQRT_HALF)
    z = (z - 0.5) / (f * 0.5 + 0.5);
  else
    {
      N--;
      z /= (z * 0.5 + 0.5);
    }
  w = z * z;

  /* Use Newton's method with 4 terms. */
  z += z * w * ((logarithm_a[2] * w + logarithm_a[1]) * w + logarithm_a[0]) / (((w + logarithm_b[2]) * w + logarithm_b[1]) * w + logarithm_b[0]);

  if (N != 0)
    z = (N * logarithm_C2 + z) + N * logarithm_C1;

  if (ten)
    z *= logarithm_C3;

  return (z);
}
 
double log(double x)
{
  return (logarithm (x, 0));
}

double log10(double x)
{
  return (logarithm (x, 1));
}
 

int numtest(double x)
{
  __uint32_t hx, lx;
  int exp;

  EXTRACT_WORDS (hx, lx, x);

  exp = (hx & 0x7ff00000) >> 20;

  /* Check for a zero input. */
  if (x == 0.0)
    {
      return (0);
    }

  /* Check for not a number or infinity. */
  if (exp == 0x7ff)
    {
      if(hx & 0xf0000 || lx)
        return (NAN);
      else
        return (INF);
    }
     
  /* Otherwise it's a finite value. */ 
  else
    return (NUM);
}

int numtestf(float x)
{
  __int32_t wx;
  int exp;

  GET_FLOAT_WORD (wx, x);

  exp = (wx & 0x7f800000) >> 23;

  /* Check for a zero input. */
  if (x == 0.0)
    {
      return (0);
    }

  /* Check for not a number or infinity. */
  if (exp == 0x7f8)
    {
      if(wx & 0x7fffff)
        return (NAN);
      else
        return (INF);
    }
     
  /* Otherwise it's a finite value. */ 
  else
    return (NUM);
}
 

double ldexp(double d, int e)
{
  int exp;
  __uint32_t hd;

  GET_HIGH_WORD (hd, d);

  /* Check for special values and then scale d by e. */
  switch (numtest (d))
    {
      case NAN:
        errno = EDOM;
        break;

      case INF:
        errno = ERANGE;
        break;

      case 0:
        break;

      default:
        exp = (hd & 0x7ff00000) >> 20;
        exp += e;

        if (exp > DBL_MAX_EXP + DOUBLE_EXP_OFFS)
         {
           errno = ERANGE;
           d = z_infinity.d;
         }
        else if (exp < DBL_MIN_EXP + DOUBLE_EXP_OFFS)
         {
           errno = ERANGE;
           d = -z_infinity.d;
         }
        else
         {
           hd &= 0x800fffff;
           hd |= exp << 20;
           SET_HIGH_WORD (d, hd);
         }
    }

    return (d);
}

float ldexpf(float d, int e)
{
  int exp;
  __int32_t wd;

  GET_FLOAT_WORD (wd, d);

  /* Check for special values and then scale d by e. */
  switch (numtestf (wd))
    {
      case NAN:
        errno = EDOM;
        break;

      case INF:
        errno = ERANGE;
        break;

      case 0:
        break;

      default:
        exp = (wd & 0x7f800000) >> 23;
        exp += e;

        if (exp > FLT_MAX_EXP + FLOAT_EXP_OFFS)
         {
           errno = ERANGE;
           d = z_infinity_f.f;
         }
        else if (exp < FLT_MIN_EXP + FLOAT_EXP_OFFS)
         {
           errno = ERANGE;
           d = -z_infinity_f.f;
         }
        else
         {
           wd &= 0x807fffff;
           wd |= exp << 23;
           SET_FLOAT_WORD (d, wd);
         }
    }

    return (d);
}
 
 
float modff(float x, float *iptr)
{
	__int32_t i0,j0;
	__uint32_t i;
	GET_FLOAT_WORD(i0,x);
	j0 = ((i0>>23)&0xff)-0x7f;	/* exponent of x */
	if(j0<23) {			/* integer part in x */
	    if(j0<0) {			/* |x|<1 */
	        SET_FLOAT_WORD(*iptr,i0&0x80000000);	/* *iptr = +-0 */
		return x;
	    } else {
		i = (0x007fffff)>>j0;
		if((i0&i)==0) {			/* x is integral */
		    __uint32_t ix;
		    *iptr = x;
		    GET_FLOAT_WORD(ix,x);
		    SET_FLOAT_WORD(x,ix&0x80000000);	/* return +-0 */
		    return x;
		} else {
		    SET_FLOAT_WORD(*iptr,i0&(~i));
		    return x - *iptr;
		}
	    }
	} else {			/* no fraction part */
	    __uint32_t ix;
	    *iptr = x*oneF;
	    GET_FLOAT_WORD(ix,x);
	    SET_FLOAT_WORD(x,ix&0x80000000);	/* return +-0 */
	    return x;
	}
}
 
double modf(double x, double *iptr)
{
	__int32_t i0,i1,j0;
	__uint32_t i;
	EXTRACT_WORDS(i0,i1,x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;	/* exponent of x */
	if(j0<20) {			/* integer part in high x */
	    if(j0<0) {			/* |x|<1 */
	        INSERT_WORDS(*iptr,i0&0x80000000,0);	/* *iptr = +-0 */
		return x;
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) {		/* x is integral */
		    __uint32_t high;
		    *iptr = x;
		    GET_HIGH_WORD(high,x);
		    INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
		    return x;
		} else {
		    INSERT_WORDS(*iptr,i0&(~i),0);
		    return x - *iptr;
		}
	    }
	} else if (j0>51) {		/* no fraction part */
	    __uint32_t high;
	    *iptr = x*oneD;
	    GET_HIGH_WORD(high,x);
	    INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
	    return x;
	} else {			/* fraction part in low x */
	    i = ((__uint32_t)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) { 		/* x is integral */
	        __uint32_t high;
		*iptr = x;
		GET_HIGH_WORD(high,x);
		INSERT_WORDS(x,high&0x80000000,0);	/* return +-0 */
		return x;
	    } else {
	        INSERT_WORDS(*iptr,i0,i1&(~i));
		return x - *iptr;
	    }
	}
}


float powf(float x, float y)
{
  float d, k, t, r = 1.0;
  int n, sign, exponent_is_even_int = 0;
  int px;

  GET_FLOAT_WORD (px, x);

  k = modff (y, &d);

  if (k == 0.0) 
    {
      /* Exponent y is an integer. */
      if (modff (ldexpf (y, -1), &t))
        {
          /* y is odd. */
          exponent_is_even_int = 0;
        }
      else
        {
          /* y is even. */
          exponent_is_even_int = 1;
        }
    }

  if (x == 0.0)
    {
      if (y <= 0.0)
        errno = EDOM;
    }
  else if ((t = y * log (fabsf (x))) >= BIGX) 
    {
      errno = ERANGE;
      if (px & 0x80000000) 
        {
          /* x is negative. */
          if (k) 
            {
              /* y is not an integer. */
              errno = EDOM;
              x = 0.0;
            }
          else if (exponent_is_even_int)
            x = z_infinity_f.f;
          else
            x = -z_infinity_f.f;
        }
    else
      {
        x = z_infinity_f.f;
      }
    }
  else if (t < SMALLX)
    {
      errno = ERANGE;
      x = 0.0;
    }
  else 
    {
      if ( !k && fabsf (d) <= 32767 ) 
        {
          n = (int) d;

          if ((sign = (n < 0)))
            n = -n;

          while ( n > 0 ) 
            {
              if ((unsigned int) n % 2) 
                r *= x;
              x *= x;
              n = (unsigned int) n / 2;
            }

          if (sign)
            r = 1.0 / r;

          return r;
        }
      else 
        {
          if ( px & 0x80000000 ) 
            {
              /* x is negative. */
              if (k) 
                {
                  /* y is not an integer. */
                  errno = EDOM;
                  return 0.0;
                }
            }

          x = exp (t);

          if (!exponent_is_even_int) 
            { 
              if (px & 0x80000000)
                {
                  /* y is an odd integer, and x is negative,
                     so the result is negative. */
                  GET_FLOAT_WORD (px, x);
                  px |= 0x80000000;
                  SET_FLOAT_WORD (x, px);
                }
            }
        }
    }

  return x; 
}

double powd(double x, double y)
{
  double d, k, t, r = 1.0;
  int n, sign, exponent_is_even_int = 0;
  __uint32_t px;

  GET_HIGH_WORD (px, x);

  k = modf (y, &d);

  if (k == 0.0)
    {
      /* Exponent y is an integer. */
      if (modf (ldexp (y, -1), &t))
        {
          /* y is odd. */
          exponent_is_even_int = 0;
        }
      else
        {
          /* y is even. */
          exponent_is_even_int = 1;
        }
    }

  if (x == 0.0)
    {
      if (y <= 0.0)
        errno = EDOM;
    }
  else if ((t = y * log (fabs (x))) >= BIGX) 
    {
      errno = ERANGE;
      if (px & 0x80000000) 
        {
          /* x is negative. */
          if (k) 
            {
              /* y is not an integer. */
              errno = EDOM;
              x = 0.0;
            }
          else if (exponent_is_even_int)
            x = z_infinity.d;
          else
            x = -z_infinity.d;
        }
      else
        {
          x = z_infinity.d;
        }
    }
  else if (t < SMALLX)
    {
      errno = ERANGE;
      x = 0.0;
    }
  else 
    {
      if ( !k && fabs(d) <= 32767 ) 
        {
          n = (int) d;

          if ((sign = (n < 0)))
            n = -n;

          while ( n > 0 ) 
            {
              if ((unsigned int) n % 2) 
                r *= x;
              x *= x;
              n = (unsigned int) n / 2;
            }

          if (sign)
            r = 1.0 / r;

          return r;
        }
      else 
        {
          if ( px & 0x80000000 ) 
            {
              /* x is negative. */
              if ( k ) 
                {
                  /* y is not an integer. */
                  errno = EDOM;
                  return 0.0;
                }
            }

          x = exp (t);

          if (!exponent_is_even_int)
            {
              if (px & 0x80000000)
                {
                  /* y is an odd integer, and x is negative,
                     so the result is negative. */
                  GET_HIGH_WORD (px, x);
                  px |= 0x80000000;
                  SET_HIGH_WORD (x, px);
                }
            }
        }
    }

  return x; 
}

/*******************************************************************/
static const double sine_HALF_PI = 1.57079632679489661923;
static const double sine_ONE_OVER_PI = 0.31830988618379067154;
static const double sine_r[] = { -0.16666666666666665052, 0.83333333333331650314e-02, -0.19841269841201840457e-03,
                             0.27557319210152756119e-05, -0.25052106798274584544e-07, 0.16058936490371589114e-09,
                            -0.76429178068910467734e-12,  0.27204790957888846175e-14 };

double sine(double x, int cosine)
{
  int sgn, N;
  double y, XN, g, R, res;
  double YMAX = 210828714.0;

  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = EDOM;
        return (z_notanum.d); 
    }

  /* Use sin and cos properties to ease computations. */
  if (cosine)
    {
      sgn = 1;
      y = fabs (x) + sine_HALF_PI;
    }
  else
    {
      if (x < 0.0)
        {
          sgn = -1;
          y = -x;
        }
      else
        {
          sgn = 1;
          y = x;
        }
    }

  /* Check for values of y that will overflow here. */
  if (y > YMAX)
    {
      errno = ERANGE;
      return (x);
    }

  /* Calculate the exponent. */
  if (y < 0.0)
    N = (int) (y * sine_ONE_OVER_PI - 0.5);
  else
    N = (int) (y * sine_ONE_OVER_PI + 0.5);
  XN = (double) N;

  if (N & 1)
    sgn = -sgn;

  if (cosine)
    XN -= 0.5;

  y = fabs (x) - XN * __PI;

  if (-z_rooteps < y && y < z_rooteps)
    res = y;

  else
    {
      g = y * y;

      /* Calculate the Taylor series. */
      R = (((((((sine_r[6] * g + sine_r[5]) * g + sine_r[4]) * g + sine_r[3]) * g + sine_r[2]) * g + sine_r[1]) * g + sine_r[0]) * g);

      /* Finally, compute the result. */
      res = y + y * R;
    }
 
  res *= sgn;

  return (res);
} 

double sin(double x)
{
  return (sine (x, 0));
}


static const float sinef_HALF_PI = 1.570796326;
static const float sinef_ONE_OVER_PI = 0.318309886;
static const float sinef_r[] = { -0.1666665668, 0.8333025139e-02, -0.1980741872e-03, 0.2601903036e-5 };

float sinef(float x, int cosine)
{
  int sgn, N;
  float y, XN, g, R, res;
  float YMAX = 210828714.0;

  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = EDOM;
        return (z_notanum_f.f); 
    }

  /* Use sin and cos properties to ease computations. */
  if (cosine)
    {
      sgn = 1;
      y = fabsf (x) + sinef_HALF_PI;
    }
  else
    {
      if (x < 0.0)
        {
          sgn = -1;
          y = -x;
        }
      else
        {
          sgn = 1;
          y = x;
        }
    }

  /* Check for values of y that will overflow here. */
  if (y > YMAX)
    {
      errno = ERANGE;
      return (x);
    }

  /* Calculate the exponent. */
  if (y < 0.0)
    N = (int) (y * sinef_ONE_OVER_PI - 0.5);
  else
    N = (int) (y * sinef_ONE_OVER_PI + 0.5);
  XN = (float) N;

  if (N & 1)
    sgn = -sgn;

  if (cosine)
    XN -= 0.5;

  y = fabsf (x) - XN * __PI;

  if (-z_rooteps_f < y && y < z_rooteps_f)
    res = y;

  else
    {
      g = y * y;

      /* Calculate the Taylor series. */
      R = (((sinef_r[3] * g + sinef_r[2]) * g + sinef_r[1]) * g + sinef_r[0]) * g;

      /* Finally, compute the result. */
      res = y + y * R;
    }
 
  res *= sgn;

  return (res);
} 

float sinf(float x)
{
  return (sinef (x, 0));
}
 
double cos(double x)
{
  return (sine (x, 1));
}
 
float cosf(float x)
{
  return (sinef (x, 1));
} 

static const double tan_TWO_OVER_PI = 0.63661977236758134308;
static const double tan_p[] = { -0.13338350006421960681, 0.34248878235890589960e-2, -0.17861707342254426711e-4 };
static const double tan_q[] = { -0.46671683339755294240, 0.25663832289440112864e-1, -0.31181531907010027307e-3, 0.49819433993786512270e-6 };

double tan(double x)
{
  double y, f, g, XN, xnum, xden, res;
  int N;

  /* Check for special values. */
  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = EDOM;
        return (z_notanum.d);
    }

  y = fabs (x);

  /* Check for values that are out of our range. */
  if (y > 105414357.0)
    {
      errno = ERANGE;
      return (y);
    }

  if (x < 0.0)
    N = (int) (x * tan_TWO_OVER_PI - 0.5);
  else
    N = (int) (x * tan_TWO_OVER_PI + 0.5);

  XN = (double) N;

  f = x - N * __PI_OVER_TWO;

  /* Check for values that are too small. */
  if (-z_rooteps < f && f < z_rooteps)
    {
      xnum = f;
      xden = 1.0;
    }

  /* Calculate the polynomial. */ 
  else
    { 
      g = f * f;

      xnum = f * ((tan_p[2] * g + tan_p[1]) * g + tan_p[0]) * g + f;
      xden = (((tan_q[3] * g + tan_q[2]) * g + tan_q[1]) * g + tan_q[0]) * g + 1.0;
    }

  if (N & 1)
    {
      xnum = -xnum;
      res = xden / xnum;
    } 
  else
    {
      res = xnum / xden;
    }

  return (res);
}

static const float tanf_TWO_OVER_PI = 0.6366197723;
static const float tanf_p[] = { -0.958017723e-1 };
static const float tanf_q[] = { -0.429135777, 0.971685835e-2 };

float tanf(float x)
{
  float y, f, g, XN, xnum, xden, res;
  int N;

  /* Check for special values. */
  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        errno = EDOM;
        return (z_notanum_f.f);
    }

  y = fabsf (x);

  /* Check for values that are out of our range. */
  if (y > 105414357.0)
    {
      errno = ERANGE;
      return (y);
    }

  if (x < 0.0)
    N = (int) (x * tanf_TWO_OVER_PI - 0.5);
  else
    N = (int) (x * tanf_TWO_OVER_PI + 0.5);

  XN = (float) N;

  f = x - N * __PI_OVER_TWO;

  /* Check for values that are too small. */
  if (-z_rooteps_f < f && f < z_rooteps_f)
    {
      xnum = f;
      xden = 1.0;
    }

  /* Calculate the polynomial. */ 
  else
    { 
      g = f * f;

      xnum = f * (tanf_p[0] * g) + f;
      xden = (tanf_q[1] * g + tanf_q[0]) * g + 1.0;
    }

  /* Check for odd or even values. */
  if (N & 1)
    {
      xnum = -xnum;
      res = xden / xnum;
    } 
  else
    {
      res = xnum / xden;
    }

  return (res);
}

static const double asine_p[] = { -0.27368494524164255994e+2,
                             0.57208227877891731407e+2,
                            -0.39688862997404877339e+2,
                             0.10152522233806463645e+2,
                            -0.69674573447350646411 };
static const double asine_q[] = { -0.16421096714498560795e+3,
                             0.41714430248260412556e+3,
                            -0.38186303361750149284e+3,
                             0.15095270841030604719e+3,
                            -0.23823859153670238830e+2 };
static const double asine_a[] = { 0.0, 0.78539816339744830962 };
static const double asine_b[] = { 1.57079632679489661923, 0.78539816339744830962 };

double asine(double x, int acosine)
{
  int flag, i;
  int branch = 0;
  double g = 0., res = 0., R, P, Q, y;

  /* Check for special values. */
  i = numtest (x);
  if (i == NAN || i == INF)
    {
      errno = EDOM;
      if (i == NAN)
        return (x);
      else
        return (z_infinity.d);
    }

  y = fabs (x);
  flag = acosine;

  if (y > 0.5)
    {
      i = 1 - flag;

      /* Check for range error. */
      if (y > 1.0)
        {
          errno = ERANGE;
          return (z_notanum.d);
        }

      g = (1 - y) / 2.0;
      y = -2 * sqrt (g);
      branch = 1;
    }
  else
    {
      i = flag;
      if (y < z_rooteps)
        res = y;
      else
        g = y * y;
    }

  if (y >= z_rooteps || branch == 1)
    {
      /* Calculate the Taylor series. */
      P = ((((asine_p[4] * g + asine_p[3]) * g + asine_p[2]) * g + asine_p[1]) * g + asine_p[0]) * g;
      Q = ((((g + asine_q[4]) * g + asine_q[3]) * g + asine_q[2]) * g + asine_q[1]) * g + asine_q[0];
      R = P / Q;

      res = y + y * R;
    }

  /* Calculate asine or acose. */
  if (flag == 0)
    {
      res = (asine_a[i] + res) + asine_a[i];
      if (x < 0.0)
        res = -res;
    }
  else
    {
      if (x < 0.0)
        res = (asine_b[i] + res) + asine_b[i];
      else
        res = (asine_a[i] - res) + asine_a[i];
    }

  return (res);
}
 
double asin(double x)
{
  return (asine (x, 0));
} 

double acos(double x)
{
  return (asine (x, 1));
}

static const double atangent_ROOT3 = 1.73205080756887729353;
static const double atangent_a[] = { 0.0, 0.52359877559829887308, 1.57079632679489661923, 1.04719755119659774615 };
static const double atangent_q[] = { 0.41066306682575781263e+2, 0.86157349597130242515e+2, 0.59578436142597344465e+2, 0.15024001160028576121e+2 };
static const double atangent_p[] = { -0.13688768894191926929e+2, -0.20505855195861651981e+2, -0.84946240351320683534e+1, -0.83758299368150059274 };

double atangent(double x, double v, double u, int arctan2)
{
  double f, g, R, P, Q, A, res;
  int N;
  int branch = 0;
  int expv, expu;

  /* Preparation for calculating arctan2. */
  if (arctan2)
    {
      if (u == 0.0)
      {
        if (v == 0.0)
          {
            errno = ERANGE;
            return (z_notanum.d);
          }
        else
          {
            branch = 1;
            res = __PI_OVER_TWO;
          }
      }
      if (!branch)
        {
          int e;
          /* Get the exponent values of the inputs. */
          g = frexp (v, &expv);
          g = frexp (u, &expu);

          /* See if a divide will overflow. */
          e = expv - expu;
          if (e > DBL_MAX_EXP)
            {
               branch = 1;
               res = __PI_OVER_TWO;
            }

          /* Also check for underflow. */
          else if (e < DBL_MIN_EXP)
            {
               branch = 2;
               res = 0.0;
            }
         }
    }

  if (!branch)
    {
      if (arctan2)
        f = fabs (v / u);
      else
        f = fabs (x);

      if (f > 1.0)
        {
          f = 1.0 / f;
          N = 2;
        }
      else
        N = 0;

      if (f > (2.0 - atangent_ROOT3))
        {
          A = atangent_ROOT3 - 1.0;
          f = (((A * f - 0.5) - 0.5) + f) / (atangent_ROOT3 + f);
          N++;
        }

      /* Check for values that are too small. */
      if (-z_rooteps < f && f < z_rooteps)
        res = f;

      /* Calculate the Taylor series. */
      else
        {
          g = f * f;
          P = (((atangent_p[3] * g + atangent_p[2]) * g + atangent_p[1]) * g + atangent_p[0]) * g;
          Q = (((g + atangent_q[3]) * g + atangent_q[2]) * g + atangent_q[1]) * g + atangent_q[0];
          R = P / Q;

          res = f + f * R;
        }

      if (N > 1)
        res = -res;

      res += atangent_a[N];
    }

  if (arctan2)
    {
      if (u < 0.0)
        res = __PI - res;
      if (v < 0.0)
        res = -res;
    }
  else if (x < 0.0)
    {
      res = -res;
    }

  return (res);
}
 
double atan(double x)
{
  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        /* this should check to see if neg NaN or pos NaN... */
        return (__PI_OVER_TWO);
      case 0:
        return (0.0);
      default:
        return (atangent (x, 0, 0, 0));
    }
}
 
static const float asinef_p[] = { 0.933935835, -0.504400557 };
static const float asinef_q[] = { 0.560363004e+1, -0.554846723e+1 };
static const float asinef_a[] = { 0.0, 0.785398163 };
static const float asinef_b[] = { 1.570796326, 0.785398163 };

float asinef(float x, int acosine)
{
  int flag, i;
  int branch = 0;
  float g = 0.f, res = 0.f, R, P, Q, y;

  /* Check for special values. */
  i = numtestf (x);
  if (i == NAN || i == INF)
    {
      errno = EDOM;
      if (i == NAN)
        return (x);
      else
        return (z_infinity_f.f);
    }

  y = fabsf (x);
  flag = acosine;

  if (y > 0.5)
    {
      i = 1 - flag;

      /* Check for range error. */
      if (y > 1.0)
        {
          errno = ERANGE;
          return (z_notanum_f.f);
        }

      g = (1 - y) / 2.0;
      y = -2 * sqrtf(g);
      branch = 1;
    }
  else
    {
      i = flag;
      if (y < z_rooteps_f)
        res = y;
      else
        g = y * y;
    }

  if (y >= z_rooteps_f || branch == 1)
    {
      /* Calculate the Taylor series. */
      P = (asinef_p[1] * g + asinef_p[0]) * g;
      Q = (g + asinef_q[1]) * g + asinef_q[0];
      R = P / Q;

      res = y + y * R;
    }

  /* Calculate asine or acose. */
  if (flag == 0)
    {
      res = (asinef_a[i] + res) + asinef_a[i];
      if (x < 0.0)
        res = -res;
    }
  else
    {
      if (x < 0.0)
        res = (asinef_b[i] + res) + asinef_b[i];
      else
        res = (asinef_a[i] - res) + asinef_a[i];
    }

  return (res);
} 

float asinf(float x)
{
  return (asinef (x, 0));
}

float acosf(float x)
{
  return (asinef (x, 1));
}

static const float atangentf_ROOT3 = 1.732050807;
static const float atangentf_a[] = { 0.0, 0.523598775, 1.570796326, 1.047197551 };
static const float atangentf_q[] = { 0.1412500740e+1 };
static const float atangentf_p[] = { -0.4708325141, -0.5090958253e-1 };

float atangentf(float x, float v, float u, int arctan2)
{
  float f, g, R, P, Q, A, res;
  int N;
  int branch = 0;
  int expv, expu;

  /* Preparation for calculating arctan2. */
  if (arctan2)
    {
      if (u == 0.0)
      {
        if (v == 0.0)
          {
            errno = ERANGE;
            return (z_notanum_f.f);
          }
        else
          {
            branch = 1;
            res = __PI_OVER_TWO;
          }
      }
      if (!branch)
        {
          int e;
          /* Get the exponent values of the inputs. */
          g = frexpf (v, &expv);
          g = frexpf (u, &expu);

          /* See if a divide will overflow. */
          e = expv - expu;
          if (e > FLT_MAX_EXP)
            {
               branch = 1;
               res = __PI_OVER_TWO;
            }

          /* Also check for underflow. */
          else if (e < FLT_MIN_EXP)
            {
               branch = 2;
               res = 0.0;
            }
         }
    }

  if (!branch)
    {
      if (arctan2)
        f = fabsf (v / u);
      else
        f = fabsf (x);

      if (f > 1.0)
        {
          f = 1.0 / f;
          N = 2;
        }
      else
        N = 0;

      if (f > (2.0 - atangentf_ROOT3))
        {
          A = atangentf_ROOT3 - 1.0;
          f = (((A * f - 0.5) - 0.5) + f) / (atangentf_ROOT3 + f);
          N++;
        }

      /* Check for values that are too small. */
      if (-z_rooteps_f < f && f < z_rooteps_f)
        res = f;

      /* Calculate the Taylor series. */
      else
        {
          g = f * f;
          P = (atangentf_p[1] * g + atangentf_p[0]) * g;
          Q = g + atangentf_q[0];
          R = P / Q;

          res = f + f * R;
        }

      if (N > 1)
        res = -res;

      res += atangentf_a[N];
    }

  if (arctan2)
    {
      if (u < 0.0)
        res = __PI - res;
      if (v < 0.0)
        res = -res;
    }
  else if (x < 0.0)
    {
      res = -res;
    }

  return (res);
} 

float atanf(float x)
{
  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        /* this should check to see if neg NaN or pos NaN... */
        return (__PI_OVER_TWO);
      case 0:
        return (0.0);
      default:
        return (atangentf (x, 0, 0, 0));
    }
}

double sqrt(double x)
{
  double f, y;
  int exp, i, odd;

  /* Check for special values. */
  switch (numtest (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        if (ispos (x))
          {
            errno = EDOM;
            return (z_notanum.d);
          }
        else
          {
            errno = ERANGE;
            return (z_infinity.d);
          }
    }

  /* Initial checks are performed here. */
  if (x == 0.0)
    return (0.0);
  if (x < 0)
    {
      errno = EDOM;
      return (z_notanum.d);
    }

  /* Find the exponent and mantissa for the form x = f * 2^exp. */
  f = frexp (x, &exp);

  odd = exp & 1;

  /* Get the initial approximation. */
  y = 0.41731 + 0.59016 * f;

  f /= 2.0;
  /* Calculate the remaining iterations. */
  for (i = 0; i < 3; ++i)
    y = y / 2.0 + f / y;

  /* Calculate the final value. */
  if (odd)
    {
      y *= __SQRT_HALF;
      exp++;
    }
  exp >>= 1;
  y = ldexp (y, exp);

  return (y);
}
 

float sqrtf(float x)
{
  float f, y;
  int exp, i, odd;

  /* Check for special values. */
  switch (numtestf (x))
    {
      case NAN:
        errno = EDOM;
        return (x);
      case INF:
        if (isposf (x))
          {
            errno = EDOM;
            return (z_notanum_f.f);
          }
        else
          {
            errno = ERANGE;
            return (z_infinity_f.f);
          }
    } 

  /* Initial checks are performed here. */
  if (x == 0.0)
    return (0.0);
  if (x < 0)
    {
      errno = EDOM;
      return (z_notanum_f.f);
    }

  /* Find the exponent and mantissa for the form x = f * 2^exp. */
  f = frexpf (x, &exp);
  odd = exp & 1;

  /* Get the initial approximation. */
  y = 0.41731 + 0.59016 * f;

  f *= 0.5;
  /* Calculate the remaining iterations. */
  for (i = 0; i < 2; ++i)
    y = y * 0.5 + f / y;

  /* Calculate the final value. */
  if (odd)
    {
      y *= __SQRT_HALF;
      exp++;
    }
  exp >>= 1;
  y = ldexpf (y, exp);

  return (y);
}
 
extern unsigned long MathHelperSqrt32(unsigned long value)
{
  register unsigned long bit, trial;
  register unsigned long root = 0;
  
  for(bit = 1L<<30; bit > 0; bit>>=2)
  {
    trial = root + bit;
    root >>= 1;
    if(trial <= value)
    {
      root += bit;
      value -= trial;
    }
  }
  return root;
}
 
extern unsigned long MathHelperSqrt64(unsigned long lo, unsigned long hi)
{
  register unsigned long long bit, trial;
  unsigned long long    root  = 0;
  register unsigned long long value = hi;
  
  value = (value << 32) | lo;
  
  for(bit = 1LL<<62; bit > 0; bit>>=2)
  {
    trial = root + bit;
    root >>= 1;
    if(trial <= value)
    {
      root += bit;
      value -= trial;
    }
  }
  return root;
}
 
// ********************************************************************************************
// ST Funktionen für Math
// ********************************************************************************************


#ifdef _USE_DIAS_R_FUNCTION

float DIAS_R_SIN_GR(float a)
{
  return sinf(a * FLT_PI / 180.f);
}

float DIAS_R_SIN(float a)
{
  return sinf(a);
}

float DIAS_R_COS_GR(float a)
{
  return cosf(a * FLT_PI / 180.f);
}

float DIAS_R_COS(float a)
{
  return cosf(a);
}

float DIAS_R_TAN_GR(float a)
{
  return tanf(a * FLT_PI / 180.f);
}

float DIAS_R_TAN(float a)
{
  return tanf(a);
}

float DIAS_R_ASIN_GR(float a)
{
  return asinf(a * FLT_PI / 180.f);
}

float DIAS_R_ASIN(float a)
{
  return asinf(a);
}

float DIAS_R_ACOS_GR(float a)
{
  return acosf(a * FLT_PI / 180.f);
}

float DIAS_R_ACOS(float a)
{
  return acosf(a);
}

float DIAS_R_ATAN_GR(float a)
{
  return atanf(a * FLT_PI / 180.f );
}

float DIAS_R_ATAN(float a)
{
  return atanf(a);
}

float DIAS_R_SQRT(float a)
{
  return sqrtf(a);
}

float DIAS_R_LN(float a)
{
  return logf(a);
}

float DIAS_R_LOG(float a)
{
  return log10f(a);
}

float DIAS_R_EXP(float a)
{
  return expf(a);
}

//    X hoch Y
//    ========
//    
//    Wenn X negativ ist, wird das Ergebnis nur ungenau berechnet, da
//    in diesem Fall der Exponent als ganze Zahl betrachtet wird.
//    x^y = e^(y*ln(x))
//    
//    -> EAX: Wert vom Typ REAL (positiv)
//    -> EBX: Wert vom Typ REAL
//    
//    <- EAX: Ergebnis vom Typ REAL
float DIAS_R_XHY(float a, float b)
{
  if (a < 0.)
  {
    return powf(a, (float)(int)b);
  }
  return powf(a, b);
}

#endif // _USE_DIAS_R_FUNCTION


double DIAS_L_SIN_GR(double a)
{
  return sin(a * FLT_PI / 180.f);
}


double DIAS_L_SIN(double a)
{
  return sin(a);
}


double DIAS_L_COS_GR(double a)
{
  return cos(a * FLT_PI / 180.f);
}


double DIAS_L_COS(double a)
{
  return cos(a);
}


double DIAS_L_TAN_GR(double a)
{
  return tan(a * FLT_PI / 180.f);
}


double DIAS_L_TAN(double a)
{
  return tan(a);
}


double DIAS_L_ASIN_GR(double a)
{
  return asin(a * FLT_PI / 180.f);
}


double DIAS_L_ASIN(double a)
{
  return asin(a);
}

double DIAS_L_ACOS_GR(double a)
{
  return acos(a * FLT_PI / 180.f);
}

double DIAS_L_ACOS(double a)
{
  return acos(a);
}

double DIAS_L_ATAN_GR(double a)
{
  return atan(a * FLT_PI / 180.f );
}

double DIAS_L_ATAN(double a)
{
  return atan(a);
}

double DIAS_L_SQRT(double a)
{
  return sqrt(a);
}

double DIAS_L_LN(double a)
{
  return log(a);
}

double DIAS_L_LOG(double a)
{
  return log10(a);
}

double DIAS_L_EXP(double a)
{
  return exp(a);
}

//    X hoch Y
//    ========
//    
//    Wenn X negativ ist, wird das Ergebnis nur ungenau berechnet, da
//    in diesem Fall der Exponent als ganze Zahl betrachtet wird.
//    x^y = e^(y*ln(x))
//    
//    -> EAX: Wert vom Typ REAL (positiv)
//    -> EBX: Wert vom Typ REAL
//    
//    <- EAX: Ergebnis vom Typ REAL
double DIAS_L_XHY(double a, double b)
{
  if (a < 0.)
  {
    return powd(a, (double)(int)b);
  }
  return powd(a, b);
}

double testabs(double a)
{
  return fabs(a);
}

// ********************************************************************************************
#ifdef __cplusplus
}
#endif
