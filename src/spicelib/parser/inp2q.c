/**********
Copyright 1990 Regents of the University of California.  All rights reserved.
Author: 1988 Thomas L. Quarles
Modified: 2001 Paolo Nenzi (Cider Integration)
**********/

#include "ngspice.h"
#include <stdio.h>
#include "ifsim.h"
#include "inpdefs.h"
#include "inpmacs.h"
#include "fteext.h"
#include "inp.h"

void INP2Q(void *ckt, INPtables * tab, card * current, void *gnode)
{

    /* Qname <node> <node> <node> [<node>] <model> [<val>] [OFF]
     *       [IC=<val>,<val>] */

    int mytype;			/* the type we looked up */
    int type;			/* the type the model says it is */
    char *line;			/* the part of the current line left to parse */
    char *name;			/* the resistor's name */
    char *nname1;		/* the first node's name */
    char *nname2;		/* the second node's name */
    char *nname3;		/* the third node's name */
    char *nname4;		/* the fourth node's name */
    char *nname5;		/* the fifth node's name */
    void *node1;		/* the first node's node pointer */
    void *node2;		/* the second node's node pointer */
    void *node3;		/* the third node's node pointer */
    void *node4;		/* the fourth node's node pointer */
    void *node5;		/* the fifth node's node pointer */
    int error;			/* error code temporary */
    int nodeflag;		/* flag indicating 4 or 5 nodes */
    void *fast;			/* pointer to the actual instance */
    IFvalue ptemp;		/* a value structure to package resistance into */
    int waslead;		/* flag to indicate that funny unlabeled number was found */
    double leadval;		/* actual value of unlabeled number */
    char *model;		/* the name of the model */
    INPmodel *thismodel;	/* pointer to model description for user's model */
    void *mdfast;		/* pointer to the actual model */
    IFuid uid;			/* uid of default model */

    mytype = INPtypelook("BJT");
    if (mytype < 0) {
	LITERR("Device type BJT not supported by this binary\n");
	return;
    }
#ifdef TRACE
    printf("INP2Q: Parsing '%s'\n",current->line);
#endif

    nodeflag = 0;		/*  initially specify a 4 terminal device  */
    line = current->line;
    INPgetTok(&line, &name, 1);
    INPinsert(&name, tab);
    INPgetNetTok(&line, &nname1, 1);
    INPtermInsert(ckt, &nname1, tab, &node1);
    INPgetNetTok(&line, &nname2, 1);
    INPtermInsert(ckt, &nname2, tab, &node2);
    INPgetNetTok(&line, &nname3, 1);
    INPtermInsert(ckt, &nname3, tab, &node3);
    INPgetTok(&line, &model, 1);

    /*  See if 4th token after device specification is a model name  */
    if (INPlookMod(model)) {
	/* 3-terminal device - substrate to ground */
	node4 = gnode;
        INPinsert(&model, tab);
    } else {
	nname4 = model;
	INPtermInsert(ckt, &nname4, tab, &node4);
	INPgetTok(&line, &model, 1);
        /*  See if 5th token after device specification is a model name  */
#ifdef TRACE
        printf("INP2Q: checking for 4 node device\n");
#endif
        if (INPlookMod(model)) {
	   /* 4-terminal device - special case with tnodeout flag not handled */
           INPinsert(&model, tab);
#ifdef ADMS
        } else {
	   /* 5-terminal device */
#ifdef TRACE
           printf("INP2Q: checking for 5 node device\n");
#endif
           nodeflag = 1;		/*  now specify a 5 node device  */
           nname5 = model;
           INPtermInsert(ckt, &nname5, tab, &node5);
           INPgetTok(&line, &model, 1);
           INPinsert(&model, tab);
#endif
        }
    }

    current->error = INPgetMod(ckt, model, &thismodel, tab);
#ifdef TRACE
    printf("INP2Q: Looking up model\n");
#endif
    if (thismodel != NULL) {
	if((thismodel->INPmodType != INPtypelook("BJT"))
           && (thismodel->INPmodType != INPtypelook("BJT2"))
#ifdef CIDER
           && (thismodel->INPmodType != INPtypelook("NBJT"))
           && (thismodel->INPmodType != INPtypelook("NBJT2"))
#endif
#ifdef ADMS
           && (thismodel->INPmodType != INPtypelook("hicum0"))
           && (thismodel->INPmodType != INPtypelook("hicum2"))
           && (thismodel->INPmodType != INPtypelook("mextram"))
           && (thismodel->INPmodType != INPtypelook("vbic"))
#endif
          ) {
            LITERR("incorrect model type")
            return;
        }
#ifdef ADMS
	if (nodeflag && (thismodel->INPmodType != INPtypelook("hicum2")))
        {
            LITERR("Too much nodes for this model type")
            return;
        }
#endif
        type = (thismodel->INPmodType);
        mdfast = (thismodel->INPmodfast);    
    } else {
	type = mytype;
	if (!tab->defQmod) {
	    /* create default Q model */
	    IFnewUid(ckt, &uid, (IFuid) NULL, "Q", UID_MODEL,
		     (void **) NULL);
	    IFC(newModel, (ckt, type, &(tab->defQmod), uid));
	}
	mdfast = tab->defQmod;
    }
    
#ifdef TRACE
    printf ("INP2Q: Just about to dive into newInstance\n");
#endif
    
    IFC(newInstance, (ckt, mdfast, &fast, name));
    IFC(bindNode, (ckt, fast, 1, node1));
    IFC(bindNode, (ckt, fast, 2, node2));
    IFC(bindNode, (ckt, fast, 3, node3));
    IFC(bindNode, (ckt, fast, 4, node4));

#ifdef ADMS
     if ((type == INPtypelook ("hicum0")) ||
        (type == INPtypelook ("hicum2")) ||
        (type == INPtypelook ("mextram")) )
        {
            if (nodeflag) {
            IFC(bindNode, (ckt, fast, 5, node5));
            } else {
              ((GENinstance *) fast)->GENnode5 = -1;
            }
     }
#endif
    PARSECALL((&line, ckt, type, fast, &leadval, &waslead, tab));
    if (waslead) {
#ifdef CIDER
    if( type == INPtypelook("NBJT2") ) {
            LITERR(" error:  no unlabeled parameter permitted on NBJT2\n")
 	} else {
#endif
	ptemp.rValue = leadval;
	GCA(INPpName, ("area", &ptemp, ckt, type, fast));
    }
#ifdef CIDER
   }
#endif   
}
