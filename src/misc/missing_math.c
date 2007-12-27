/**********
Copyright 1991 Regents of the University of California.  All rights reserved.
$Id: missing_math.c,v 1.5 2007/12/27 17:02:22 dwarning Exp $
**********/

/*
 * Missing math functions
 */
#include <assert.h>
#include "config.h"
#include "ngspice.h"
#include "missing_math.h"

/* Initial AlmostEqualULPs version - fast and simple, but */
/* some limitations. */
bool AlmostEqualUlps(double A, double B, long int maxUlps)
{
    long int intDiff;
    assert(sizeof(double) == sizeof(long int));

    if (A == B)
        return TRUE;

    intDiff = abs(*(long int*)&A - *(long int*)&B);

    if (intDiff <= maxUlps)
        return TRUE;

    return FALSE;
}

#ifndef HAVE_LOGB

double
logb(double x)	
{
  double y = 0.0;
  
  if (x != 0.0) 
    {
      if (x < 0.0)
	x = - x;
      while (x > 2.0) 
	{
	  y += 1.0;
	  x /= 2.0;
	}
      while (x < 1.0) 
	{
	  y -= 1.0;
	  x *= 2.0;
	}
    } 
  else
    y = 0.0;
  
  return y;
}
#endif




#ifndef HAVE_SCALB
#  ifdef HAVE_SCALBN
#    define scalb scalbn
#else                       /* Chris Inbody */

double
scalb(double x, int n)
{
  double y, z = 1.0, k = 2.0;
  
  if (n < 0) {
    n = -n;
    k = 0.5;
  }
  
  if (x != 0.0)
    for (y = 1.0; n; n >>= 1) {
      y *= k;
      if (n & 1)
	z *= y;
    }
  
  return x * z;
}
#   endif /* HAVE_SCALBN */
#endif /* HAVE_SCALB */


#ifndef HAVE_ERFC
/* From C. Hastings, Jr., Approximations for digital computers,
	Princeton Univ. Press, 1955.
   Approximation accurate to within 1.5E-7
   (making some assumptions about your machine's floating point mechanism)
*/

double
erfc(double  x)
     
{
  double t, z;
  
  t =  1/(1 + 0.3275911*x);
  z =  1.061405429;
  z = -1.453152027 + t * z;
  z =  1.421413741 + t * z;
  z = -0.284496736 + t * z;
  z =  0.254829592 + t * z;
  z =  exp(-x*x) * t * z;
  
  return(z);
}
#endif



#ifndef HAVE_ISNAN
/* isnan (originally) for SOI devices in MINGW32  hvogt (dev.c) */

union ieee754_double
{
	double d;
	
	/* This is the IEEE 754 double-precision format.  */
	struct
	{
    /* Together these comprise the mantissa.  */
    unsigned int mantissa1:32;
    unsigned int mantissa0:20;
    unsigned int exponent:11;
    unsigned int negative:1;
	} ieee;
	struct
	{
    /* Together these comprise the mantissa.  */
    unsigned int mantissa1:32;
    unsigned int mantissa0:19;
    unsigned int quiet_nan:1;
    unsigned int exponent:11;
    unsigned int negative:1;
	} ieee_nan;
};

int
isnan(double value)

{
  union ieee754_double u;
	
  u.d = value;
	
  /* IEEE 754 NaN's have the maximum possible
		exponent and a nonzero mantissa.  */
  return ((u.ieee.exponent & 0x7ff) == 0x7ff &&
          (u.ieee.mantissa0 != 0 || u.ieee.mantissa1 != 0));
	
}

/*
 * end isnan.c
 */
#endif /* HAVE_ISNAN */

