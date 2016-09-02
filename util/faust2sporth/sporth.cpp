/************************************************************************
 ************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------

	This is sample code. This file is provided as an example of minimal
	FAUST architecture file. Redistribution and use in source and binary
	forms, with or without modification, in part or in full are permitted.
	In particular you can create a derived work of this FAUST architecture
	and distribute that work under terms of your choice.

	This sample code is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 ************************************************************************
 ************************************************************************/

#include <cmath>
#include <stdlib.h>

#include "faust/gui/UI.h"
#include "faust/dsp/dsp.h"
#include "faust/gui/meta.h"

#ifndef min
#define min(A, B) ((A < B) ? A : B)
#endif

#ifndef max
#define max(A, B) ((A > B) ? A : B)
#endif

#define FAUST_UIMACROS
#define FAUST_ADDBUTTON(l,f)
#define FAUST_ADDCHECKBOX(l,f)
#define FAUST_ADDVERTICALSLIDER(l,f,i,a,b,s)
#define FAUST_ADDHORIZONTALSLIDER(l,f,i,a,b,s)
#define FAUST_ADDNUMENTRY(l,f,i,a,b,s)
#define FAUST_ADDVERTICALBARGRAPH(l,f,a,b)
#define FAUST_ADDHORIZONTALBARGRAPH(l,f,a,b)

/******************************************************************************
*******************************************************************************

							       VECTOR INTRINSICS

*******************************************************************************
*******************************************************************************/

<<includeIntrinsic>>

/******************************************************************************
*******************************************************************************

			ABSTRACT USER INTERFACE

*******************************************************************************
*******************************************************************************/

//----------------------------------------------------------------------------
//  FAUST generated signal processor
//----------------------------------------------------------------------------

<<includeclass>>


extern "C" {
#include <soundpipe.h>
#include <sporth.h>



class Sporth_mydsp : public UI {

    private:
    void pushValue(FAUSTFLOAT *zone, FAUSTFLOAT init, const char *label) {
        printf("%d: %s (default %g) %d\n", FAUST_ACTIVES - pos, label, init, pos);
        args[FAUST_INPUTS + pos--] = zone;
    }

    public:
	virtual void openHorizontalBox(const char* label)	{ }
	virtual void openVerticalBox(const char* label)		{ }
	virtual void closeBox() 							{ }
	
    virtual void openTabBox(const char* label)			{ }

	void addOption(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max)
	{
        pushValue(zone, init, label);
	}

	virtual void addButton(const char* label, FAUSTFLOAT* zone)
	{
        pushValue(zone, 0, label);
	}

	virtual void addCheckButton(const char* label, FAUSTFLOAT* zone)
	{
        pushValue(zone, 0, label);
	}

	virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
	{
        pushValue(zone, init, label);
	}

	virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
	{
        pushValue(zone, init, label);
	}

	virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
	{
	}

	// -- passive widgets

	virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 		{}
	virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) 			{}

    int bar;
    mydsp dsp;
    SPFLOAT *args[FAUST_INPUTS + FAUST_ACTIVES];
    SPFLOAT *outs_p[FAUST_OUTPUTS];
    SPFLOAT outs[FAUST_OUTPUTS];
    SPFLOAT ins[FAUST_INPUTS];
    uint32_t pos;

};


static int sporth_mydsp(plumber_data *pd, sporth_stack *stack, void **ud)
{
    Sporth_mydsp *mydsp;
    SPFLOAT gain, in;
    SPFLOAT out;
    int i;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            fprintf(stderr, "mydsp: creating\n");

            mydsp = new Sporth_mydsp;
            *ud = (void *)mydsp;
            mydsp->pos = FAUST_ACTIVES - 1;
            mydsp->dsp.buildUserInterface(mydsp);

            for(i = FAUST_INPUTS; i < FAUST_INPUTS + FAUST_ACTIVES; i++) {
                *mydsp->args[i] = sporth_stack_pop_float(stack);
            }

            for(i = 0; i < FAUST_INPUTS; i++) {
                sporth_stack_pop_float(stack);
                mydsp->ins[i] = 0;
                mydsp->args[i] = &mydsp->ins[i];
            }

            for(i = 0; i < FAUST_OUTPUTS; i++) {
                sporth_stack_push_float(stack, 0.0);
                mydsp->outs_p[i] = &mydsp->outs[i];
            }
            mydsp->dsp.init((int) pd->sp->sr);

            break;
        case PLUMBER_INIT:
            mydsp = (Sporth_mydsp *)*ud;

            for(i = FAUST_INPUTS; i < FAUST_INPUTS + FAUST_ACTIVES; i++) {
                *mydsp->args[i] = sporth_stack_pop_float(stack);
            }

            for(i = 0; i < FAUST_INPUTS; i++) {
                *mydsp->args[i] = sporth_stack_pop_float(stack);
            }
            

            for(i = 0; i < FAUST_OUTPUTS; i++) {
                sporth_stack_push_float(stack, 0.0);
            }

            break;

        case PLUMBER_COMPUTE:

            mydsp = (Sporth_mydsp *)*ud;

            for(i = FAUST_INPUTS; i < FAUST_INPUTS + FAUST_ACTIVES; i++) {
                *mydsp->args[i] = sporth_stack_pop_float(stack);
            }

            for(i = 0; i < FAUST_INPUTS; i++) {
                *mydsp->args[i] = sporth_stack_pop_float(stack);
            }

            mydsp->dsp.compute(1, mydsp->args, mydsp->outs_p);

            for(i = 0; i < FAUST_OUTPUTS; i++) {
                sporth_stack_push_float(stack, *mydsp->outs_p[i]);
            }

            break;

        case PLUMBER_DESTROY:
            mydsp = (Sporth_mydsp *)*ud;
            delete mydsp;
            break;
        default:
            fprintf(stderr, "gain: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_mydsp;
}

}
