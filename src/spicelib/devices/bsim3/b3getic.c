/* $Id: b3getic.c,v 1.1.1.1 2000/04/27 20:03:59 pnenzi Exp $  */
/*
 $Log: b3getic.c,v $
 Revision 1.1.1.1  2000/04/27 20:03:59  pnenzi
 Imported sources

 Revision 1.1.1.1  1999/11/15 10:35:08  root
 Rework imported sources

 Revision 1.1.1.1  1999/07/30 09:05:13  root
 NG-Spice starting sources

 * Revision 3.2.2 1999/4/20  18:00:00  Weidong
 * BSIM3v3.2.2 release
 *
*/
static char rcsid[] = "$Id: b3getic.c,v 1.1.1.1 2000/04/27 20:03:59 pnenzi Exp $";

/*************************************/

/**********
Copyright 1999 Regents of the University of California.  All rights reserved.
Author: 1995 Min-Chie Jeng and Mansun Chan.
Author: 1997-1999 Weidong Liu.
File: b3getic.c
**********/

#include "ngspice.h"
#include <stdio.h>
#include "cktdefs.h"
#include "bsim3def.h"
#include "sperror.h"
#include "suffix.h"


int
BSIM3getic(inModel,ckt)
GENmodel *inModel;
CKTcircuit *ckt;
{
BSIM3model *model = (BSIM3model*)inModel;
BSIM3instance *here;

    for (; model ; model = model->BSIM3nextModel) 
    {    for (here = model->BSIM3instances; here; here = here->BSIM3nextInstance)
	 {    
	      if (here->BSIM3owner != ARCHme) continue;
	      if(!here->BSIM3icVBSGiven) 
	      {  here->BSIM3icVBS = *(ckt->CKTrhs + here->BSIM3bNode) 
				  - *(ckt->CKTrhs + here->BSIM3sNode);
              }
              if (!here->BSIM3icVDSGiven) 
	      {   here->BSIM3icVDS = *(ckt->CKTrhs + here->BSIM3dNode) 
				   - *(ckt->CKTrhs + here->BSIM3sNode);
              }
              if (!here->BSIM3icVGSGiven) 
	      {   here->BSIM3icVGS = *(ckt->CKTrhs + here->BSIM3gNode) 
				   - *(ckt->CKTrhs + here->BSIM3sNode);
              }
         }
    }
    return(OK);
}


