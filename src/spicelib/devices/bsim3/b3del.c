/* $Id: b3del.c,v 1.1 2000/04/27 20:03:59 pnenzi Exp $  */
/*
 $Log: b3del.c,v $
 Revision 1.1  2000/04/27 20:03:59  pnenzi
 Initial revision

 Revision 1.1.1.1  1999/11/15 10:35:08  root
 Rework imported sources

 Revision 1.2  1999/08/28 21:00:03  manu
 Big commit - merged ngspice.h, misc.h and util.h - protoized fte

 Revision 1.1.1.1  1999/07/30 09:05:13  root
 NG-Spice starting sources

 * Revision 3.2.2 1999/4/20  18:00:00  Weidong
 * BSIM3v3.2.2 release
 *
*/
static char rcsid[] = "$Id: b3del.c,v 1.1 2000/04/27 20:03:59 pnenzi Exp $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
Author: 1997-1999 Weidong Liu.
File: b3del.c
**********/
/*
 */

#include "ngspice.h"
#include <stdio.h>
#include "bsim3def.h"
#include "sperror.h"
#include "gendefs.h"
#include "suffix.h"


int
BSIM3delete(inModel,name,inInst)
GENmodel *inModel;
IFuid name;
GENinstance **inInst;
{
BSIM3instance **fast = (BSIM3instance**)inInst;
BSIM3model *model = (BSIM3model*)inModel;
BSIM3instance **prev = NULL;
BSIM3instance *here;

    for (; model ; model = model->BSIM3nextModel) 
    {    prev = &(model->BSIM3instances);
         for (here = *prev; here ; here = *prev) 
	 {    if (here->BSIM3name == name || (fast && here==*fast))
	      {   *prev= here->BSIM3nextInstance;
                  FREE(here);
                  return(OK);
              }
              prev = &(here->BSIM3nextInstance);
         }
    }
    return(E_NODEV);
}


