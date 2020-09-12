#include "plumber.h"

int sporth_phasewarp(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT sig;
    SPFLOAT out;
    SPFLOAT amount;
    sp_phasewarp *phasewarp;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "phasewarp: Creating\n");
#endif

            sp_phasewarp_create(&phasewarp);
            plumber_add_ugen(pd, SPORTH_PHASEWARP, phasewarp);
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for phasewarp\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            amount = sporth_stack_pop_float(stack);
            sig = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "phasewarp: Initialising\n");
#endif

            amount = sporth_stack_pop_float(stack);
            sig = sporth_stack_pop_float(stack);
            phasewarp = pd->last->ud;
            sp_phasewarp_init(pd->sp, phasewarp);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            amount = sporth_stack_pop_float(stack);
            sig = sporth_stack_pop_float(stack);
            phasewarp = pd->last->ud;
            phasewarp->amount = amount;
            sp_phasewarp_compute(pd->sp, phasewarp, &sig, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            phasewarp = pd->last->ud;
            sp_phasewarp_destroy(&phasewarp);
            break;
        default:
            plumber_print(pd, "phasewarp: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
