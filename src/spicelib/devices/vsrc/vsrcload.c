/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1985 Thomas L. Quarles
Modified: 2000 AlansFixes
$Id: vsrcload.c,v 1.7 2007/10/09 08:44:52 pnenzi Exp $
**********/

#include "ngspice.h"
#include "cktdefs.h"
#include "vsrcdefs.h"
#include "trandefs.h"
#include "sperror.h"
#include "suffix.h"

#ifdef XSPICE_EXP
/* gtri - begin - wbk - modify for supply ramping option */
#include "cmproto.h"
/* gtri - end   - wbk - modify for supply ramping option */
#endif /* XSPICE_EXP */

int
VSRCload(GENmodel *inModel, CKTcircuit *ckt)
        /* actually load the current voltage value into the 
         * sparse matrix previously provided 
         */
{
    VSRCmodel *model = (VSRCmodel *)inModel;
    VSRCinstance *here;
    double time;
    double value;

    /*  loop through all the voltage source models */
    for( ; model != NULL; model = model->VSRCnextModel ) {

        /* loop through all the instances of the model */
        for (here = model->VSRCinstances; here != NULL ;
                here=here->VSRCnextInstance) {
	    if (here->VSRCowner != ARCHme) continue;
            
            *(here->VSRCposIbrptr) += 1.0 ;
            *(here->VSRCnegIbrptr) -= 1.0 ;
            *(here->VSRCibrPosptr) += 1.0 ;
            *(here->VSRCibrNegptr) -= 1.0 ;
            if( (ckt->CKTmode & (MODEDCOP | MODEDCTRANCURVE)) &&
                    here->VSRCdcGiven ) {
                /* grab dc value */
#ifdef XSPICE_EXP
                value = here->VSRCdcValue;
#else
                value = ckt->CKTsrcFact * here->VSRCdcValue;
#endif
            } else {
                if(ckt->CKTmode & (MODEDC)) {
                    time = 0;
                } else {
                    time = ckt->CKTtime;
                }
                /* use the transient functions */
                switch(here->VSRCfunctionType) {
                default: { /* no function specified:   use the DC value */
                    value = here->VSRCdcValue;
                    break;
                }
                
                case PULSE: {
		    double	V1, V2, TD, TR, TF, PW, PER;
                    double	basetime = 0;
#ifdef XSPICE
                    double PHASE;
                    double phase;
                    double deltat;
                    double basephase;
#endif
		    V1 = here->VSRCcoeffs[0];
		    V2 = here->VSRCcoeffs[1];
		    TD = here->VSRCfunctionOrder > 2
			? here->VSRCcoeffs[2] : 0.0;
		    TR = here->VSRCfunctionOrder > 3
			&& here->VSRCcoeffs[3] != 0.0
			? here->VSRCcoeffs[3] : ckt->CKTstep;
		    TF = here->VSRCfunctionOrder > 4
			&& here->VSRCcoeffs[4] != 0.0
			? here->VSRCcoeffs[4] : ckt->CKTstep;
		    PW = here->VSRCfunctionOrder > 5
			&& here->VSRCcoeffs[5] != 0.0
			? here->VSRCcoeffs[5] : ckt->CKTfinalTime;
		    PER = here->VSRCfunctionOrder > 6
			&& here->VSRCcoeffs[6] != 0.0
			? here->VSRCcoeffs[6] : ckt->CKTfinalTime;
#ifdef XSPICE
		    /* gtri - begin - wbk - add PHASE parameter */
		    PHASE = here->VSRCfunctionOrder > 7
		         ? here->VSRCcoeffs[7] : 0.0;
			 
		    /* normalize phase to 0 - 2PI */ 
                    phase = PHASE * M_PI / 180.0;
                    basephase = 2 * M_PI * floor(phase / (2 * M_PI));
                    phase -= basephase;

                    /* compute equivalent delta time and add to time */
                    deltat = (phase / (2 * M_PI)) * PER;
                    time += deltat;
		    /* gtri - end - wbk - add PHASE parameter */
#endif		    
                    time -= TD;
                    if(time > PER) {
                        /* repeating signal - figure out where we are */
                        /* in period */
                        basetime = PER * floor(time/PER);
                        time -= basetime;
                    }
                    if (time <= 0 || time >= TR + PW + TF) {
                        value = V1;
                    } else  if (time >= TR && time <= TR + PW) {
                        value = V2;
                    } else if (time > 0 && time < TR) {
                        value = V1 + (V2 - V1) * (time) / TR;
                    } else { /* time > TR + PW && < TR + PW + TF */
                        value = V2 + (V1 - V2) * (time - (TR + PW)) / TF;
                    }

                }
                break;

                case SINE: {
		
		    double VO, VA, FREQ, TD, THETA;
/* gtri - begin - wbk - add PHASE parameter */
#ifdef XSPICE
                    double PHASE;
		    double phase;

                    PHASE = here->VSRCfunctionOrder > 5
		           ? here->VSRCcoeffs[5] : 0.0;
		       		
	     	    /* compute phase in radians */ 
                    phase = PHASE * M_PI / 180.0;
#endif
                    VO = here->VSRCcoeffs[0];
	            VA = here->VSRCcoeffs[1];
                    FREQ =  here->VSRCfunctionOrder > 2 
	                 && here->VSRCcoeffs[2] != 0.0
		          ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
	            TD = here->VSRCfunctionOrder > 3
	                ? here->VSRCcoeffs[3] : 0.0;
                    THETA = here->VSRCfunctionOrder > 4
	                   ? here->VSRCcoeffs[4] : 0.0;
			   
                    time -= TD;
                    if (time <= 0) {
                        value = VO;
                    } else {
#ifdef XSPICE
                        value = VO + VA * sin(FREQ*time * 2.0 * M_PI + phase) * 
                                exp(-time*THETA);
#else						    
                        value = VO + VA * sin(FREQ * time * 2.0 * M_PI) * 
                                exp(-(time*THETA));
#endif
/* gtri - end - wbk - add PHASE parameter */				
                    }
                }
                break;

                case EXP: {
		    double V1, V2, TD1, TD2, TAU1, TAU2;
                    
                    V1  = here->VSRCcoeffs[0];
		    V2  = here->VSRCcoeffs[1];
		    TD1 = here->VSRCfunctionOrder > 2 
		        && here->VSRCcoeffs[2] != 0.0
			 ? here->VSRCcoeffs[2] : ckt->CKTstep;
		    TAU1 = here->VSRCfunctionOrder > 3 
		         && here->VSRCcoeffs[3] != 0.0
			  ? here->VSRCcoeffs[3] : ckt->CKTstep;
                    TD2  = here->VSRCfunctionOrder > 4 
		         && here->VSRCcoeffs[4] != 0.0
			  ? here->VSRCcoeffs[4] : TD1 + ckt->CKTstep;
                    TAU2 = here->VSRCfunctionOrder > 5 
		         && here->VSRCcoeffs[5]
			  ? here->VSRCcoeffs[5] : ckt->CKTstep;
                    
		    
                    if(time <= TD1)  {
                        value = V1;
                    } else if (time <= TD2) {
                         value = V1 + (V2-V1)*(1-exp(-(time-TD1)/TAU1));
                    } else {
                        value = V1 + (V2-V1)*(1-exp(-(time-TD1)/TAU1)) +
                                     (V1-V2)*(1-exp(-(time-TD2)/TAU2)) ;
                    }
                }
                break;

                case SFFM:{
		
		    double VO, VA, FC, MDI, FS;
/* gtri - begin - wbk - add PHASE parameters */
#ifdef XSPICE

                    double PHASEC, PHASES;
                    double phasec;
                    double phases;
		    
                    PHASEC = here->VSRCfunctionOrder > 5
		            ? here->VSRCcoeffs[5] : 0.0;
                    PHASES = here->VSRCfunctionOrder > 6
		            ? here->VSRCcoeffs[6] : 0.0;
			
                    /* compute phases in radians */
                    phasec = PHASEC * M_PI / 180.0;
                    phases = PHASES * M_PI / 180.0;    

#endif				    
                   VO = here->VSRCcoeffs[0];
                   VA = here->VSRCcoeffs[1];
                   FC = here->VSRCfunctionOrder > 2 
		      && here->VSRCcoeffs[2]
		       ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
                   MDI = here->VSRCfunctionOrder > 3
		        ? here->VSRCcoeffs[3] : 0.0;
                   FS  = here->VSRCfunctionOrder > 4 
		       && here->VSRCcoeffs[4]
		        ? here->VSRCcoeffs[4] : (1/ckt->CKTfinalTime);
#ifdef XSPICE
                    /* compute waveform value */
                    value = VO + VA * 
                        sin((2 * M_PI * FC * time + phasec) +
                        MDI * sin(2.0 * M_PI * FS * time + phases));
#else /* XSPICE */			
                    value = VO + VA * 
                            sin((2.0 * M_PI * FC * time) +
                            MDI * sin(2 * M_PI * FS * time));
#endif /* XSPICE */
/* gtri - end - wbk - add PHASE parameters */
                }
                break;
                case AM:{
		
		double VA, FC, MF, VO, TD;
/* gtri - begin - wbk - add PHASE parameters */
#ifdef XSPICE

                    double PHASEC, PHASES;
                    double phasec;
                    double phases;
		    
                    PHASEC = here->VSRCfunctionOrder > 5
		            ? here->VSRCcoeffs[5] : 0.0;
                    PHASES = here->VSRCfunctionOrder > 6
		            ? here->VSRCcoeffs[6] : 0.0;
			
                    /* compute phases in radians */
                    phasec = PHASEC * M_PI / 180.0;
                    phases = PHASES * M_PI / 180.0;    

#endif			
		
		   VA = here->VSRCcoeffs[0];
                   VO = here->VSRCcoeffs[1];
                   MF = here->VSRCfunctionOrder > 2 
		      && here->VSRCcoeffs[2]
		       ? here->VSRCcoeffs[2] : (1/ckt->CKTfinalTime);
                   FC = here->VSRCfunctionOrder > 3
		        ? here->VSRCcoeffs[3] : 0.0;
                   TD  = here->VSRCfunctionOrder > 4 
		       && here->VSRCcoeffs[4]
		        ? here->VSRCcoeffs[4] : 0,0;

                    time -= TD;
                    if (time <= 0) {
                        value = 0;
                    } else {
#ifdef XSPICE
                    /* compute waveform value */
		    value = VA * (VO + sin(2.0 * M_PI * MF * time + phases )) *
		        sin(2 * M_PI * FC * time + phases);
                    
#else /* XSPICE */		    
		        value = VA * (VO + sin(2.0 * M_PI * MF * time)) *
		        sin(2 * M_PI * FC * time);
#endif			
		    }
		    
/* gtri - end - wbk - add PHASE parameters */
		}
		break;
                case PWL: {
                    int i = 0, num_repeat = 0;
                    double foo, repeat_time = 0, end_time, breakpt_time;
                    if(time < *(here->VSRCcoeffs)) {
                        foo = *(here->VSRCcoeffs + 1) ;
                        value = foo;
                        goto loadDone;
                    }

		    do {
		      for( ; i<(here->VSRCfunctionOrder/2)-1; i++ ) {

                        if ( fabs( (*(here->VSRCcoeffs+2*i)+repeat_time) - time ) < 1e-20 ) {
			  foo   = *(here->VSRCcoeffs+2*i+1);
			  value = foo;
			  goto loadDone;
                        } else if ( (*(here->VSRCcoeffs+2*i)+repeat_time < time) && (*(here->VSRCcoeffs+2*(i+1))+repeat_time > time) ) {
			  foo   = *(here->VSRCcoeffs+2*i+1) + (((time-(*(here->VSRCcoeffs+2*i)+repeat_time))/
								(*(here->VSRCcoeffs+2*(i+1)) - *(here->VSRCcoeffs+2*i))) *
							       (*(here->VSRCcoeffs+2*i+3)    - *(here->VSRCcoeffs+2*i+1)));
			  value = foo;
			  goto loadDone;
                        }
		      }
		      foo = *(here->VSRCcoeffs+ here->VSRCfunctionOrder-1) ;
		      value = foo;

		      if ( !here->VSRCrGiven ) goto loadDone;
		      
		      end_time     = *(here->VSRCcoeffs + here->VSRCfunctionOrder-2);
		      breakpt_time = *(here->VSRCcoeffs + here->VSRCrBreakpt);
		      repeat_time  = end_time + (end_time - breakpt_time)*num_repeat++ - breakpt_time;
		      i            = here->VSRCrBreakpt - 1;
		    } while ( here->VSRCrGiven );
                    break;
                }
                }
            }
loadDone:
/* gtri - begin - wbk - modify for supply ramping option */
#ifdef XSPICE_EXP
            value *= ckt->CKTsrcFact;
            value *= cm_analog_ramp_factor();
#else
if (ckt->CKTmode & MODETRANOP) value *= ckt->CKTsrcFact;
          *(ckt->CKTrhs + (here->VSRCbranch)) += value;
#endif
/* gtri - end - wbk - modify to process srcFact, etc. for all sources */
        }
    }
    return(OK);
}
