
/* RCS Info: $Revision: 1.2 $ on $Date: 2000/05/06 14:12:52 $
 *         $Source: /cvsroot/ngspice/ngspice/ng-spice-rework/src/include/Attic/ftedata.h,v $
 * Copyright (c) 1985 Wayne A. Christopher, U. C. Berkeley CAD Group
 *
 * Structures used for representing spice data in nutmeg.
 */

#ifndef FTEdata_h
#define FTEdata_h

#include "cpstd.h"      /* for struct complex */

/* FIXME: Adjust all callers to use the new header files. */
#warning "Please use either dvec.h or plot.h instead of ftedata.h"

#include "dvec.h"
#include "plot.h"
#endif /* FTEdata_h */
