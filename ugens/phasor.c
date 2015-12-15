#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

typedef struct {
    sp_phasor *phasor;
} sporth_fm_d;

int sporth_phasor(sporth_stack *stack, void *ud)
{
    if(stack->error > 0) return PLUMBER_NOTOK;

    plumber_data *pd = ud;
    SPFLOAT out = 0, freq, phs;
    sp_phasor *phasor;
    switch(pd->mode){
        case PLUMBER_CREATE:
#ifdef DEBUG_MODE
            fprintf(stderr, "Creating phasor function... \n");
#endif
            sp_phasor_create(&phasor);
            plumber_add_ugen(pd, SPORTH_PHASOR, phasor);
            break;
        case PLUMBER_INIT:
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                stack->error++;
               fprintf(stderr,"Invalid arguments for phasor.\n");
                return PLUMBER_NOTOK;
            }
            phasor = pd->last->ud;

            phs = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);

            sp_phasor_init(pd->sp, phasor, phs);
            sporth_stack_push_float(stack, 0.0);
            break;

        case PLUMBER_COMPUTE:
            phasor = pd->last->ud;

            phs = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);

            phasor->freq = freq;

            sp_phasor_compute(pd->sp, phasor, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            phasor = pd->last->ud;
            sp_phasor_destroy(&phasor);
            break;
        default:
            fprintf(stderr,"Error: Unknown mode!");
            stack->error++;
            return PLUMBER_NOTOK;
            break;
    }
    return PLUMBER_OK;
}
