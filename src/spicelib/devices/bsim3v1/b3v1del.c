/* $Id: b3v1del.c,v 1.1.1.1 2000/04/27 20:03:59 pnenzi Exp $  */
/* 
$Log: b3v1del.c,v $
Revision 1.1.1.1  2000/04/27 20:03:59  pnenzi
Imported sources

 * Revision 3.1  96/12/08  19:53:53  yuhua
 * BSIM3v3.1 release
 * 
*/
static char rcsid[] = "$Id: b3v1del.c,v 1.1.1.1 2000/04/27 20:03:59 pnenzi Exp $";

/*************************************/

/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
File: b3v1del.c
**********/
/*
 */

#include "ngspice.h"
#include <stdio.h>
#include "bsim3v1def.h"
#include "sperror.h"
#include "gendefs.h"
#include "suffix.h"


int
BSIM3V1delete(inModel,name,inInst)
GENmodel *inModel;
IFuid name;
GENinstance **inInst;
{
BSIM3V1instance **fast = (BSIM3V1instance**)inInst;
BSIM3V1model *model = (BSIM3V1model*)inModel;
BSIM3V1instance **prev = NULL;
BSIM3V1instance *here;

    for (; model ; model = model->BSIM3V1nextModel) 
    {    prev = &(model->BSIM3V1instances);
         for (here = *prev; here ; here = *prev) 
	 {    if (here->BSIM3V1name == name || (fast && here==*fast))
	      {   *prev= here->BSIM3V1nextInstance;
                  FREE(here);
                  return(OK);
              }
              prev = &(here->BSIM3V1nextInstance);
         }
    }
    return(E_NODEV);
}


