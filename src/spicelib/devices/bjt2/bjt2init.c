#include <config.h>

#include <devdefs.h>

#include "bjt2itf.h"
#include "bjt2ext.h"
#include "bjt2init.h"


SPICEdev BJT2info = {
    {
	"BJT2",
        "Bipolar Junction Transistor (AG)",

        &BJT2nSize,
        &BJT2nSize,
        BJT2names,

        &BJT2pTSize,
        BJT2pTable,

        &BJT2mPTSize,
        BJT2mPTable,
	DEV_DEFAULT
    },

    DEVparam      : BJT2param,
    DEVmodParam   : BJT2mParam,
    DEVload       : BJT2load,
    DEVsetup      : BJT2setup,
    DEVunsetup    : BJT2unsetup,
    DEVpzSetup    : BJT2setup,
    DEVtemperature: BJT2temp,
    DEVtrunc      : BJT2trunc,
    DEVfindBranch : NULL,
    DEVacLoad     : BJT2acLoad,
    DEVaccept     : NULL,
    DEVdestroy    : BJT2destroy,
    DEVmodDelete  : BJT2mDelete,
    DEVdelete     : BJT2delete,
    DEVsetic      : BJT2getic,
    DEVask        : BJT2ask,
    DEVmodAsk     : BJT2mAsk,
    DEVpzLoad     : BJT2pzLoad,
    DEVconvTest   : BJT2convTest,
    DEVsenSetup   : BJT2sSetup,
    DEVsenLoad    : BJT2sLoad,
    DEVsenUpdate  : BJT2sUpdate,
    DEVsenAcLoad  : BJT2sAcLoad,
    DEVsenPrint   : BJT2sPrint,
    DEVsenTrunc   : NULL,
    DEVdisto      : BJT2disto,
    DEVnoise      : BJT2noise,
                    
    DEVinstSize   : &BJT2iSize,
    DEVmodSize    : &BJT2mSize

};


SPICEdev *
get_bjt2_info(void)
{
    return &BJT2info;
}