/**********
Copyright 1999 Emmanuel Rouat
**********/


/* Decl. for missing maths functions, if any */

#ifndef MISSING_MATH_H_INCLUDED
#define MISSING_MATH_H_INCLUDED

bool AlmostEqualUlps(double, double, int);

#ifndef HAVE_ERFC
extern double erfc(double);
#endif

#ifndef HAVE_LOGB
extern double logb(double);
#endif

#ifndef HAVE_SCALB
#  ifndef HAVE_SCALBN
extern double scalb(double, int);
#endif
#endif

#ifndef HAVE_ISNAN
extern int isnan(double);
#endif

#endif /* MISSING_MATH_H_INCLUDED */
