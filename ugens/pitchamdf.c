#include "plumber.h"

int sporth_pitchamdf(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT input;
    SPFLOAT freq;
    SPFLOAT rms;
    SPFLOAT min;
    SPFLOAT max;
    sp_pitchamdf *pitchamdf;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "pitchamdf: Creating\n");
#endif

            sp_pitchamdf_create(&pitchamdf);
            plumber_add_ugen(pd, SPORTH_PITCHAMDF, pitchamdf);
            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for pitchamdf\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            min = sporth_stack_pop_float(stack);
            max = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "pitchamdf: Initialising\n");
#endif

            min = sporth_stack_pop_float(stack);
            max = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            pitchamdf = pd->last->ud;
            sp_pitchamdf_init(pd->sp, pitchamdf, min, max);
            sporth_stack_push_float(stack, 0);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            min = sporth_stack_pop_float(stack);
            max = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            pitchamdf = pd->last->ud;
            sp_pitchamdf_compute(pd->sp, pitchamdf, &input, &freq, &rms);
            sporth_stack_push_float(stack, freq);
            sporth_stack_push_float(stack, rms);
            break;
        case PLUMBER_DESTROY:
            pitchamdf = pd->last->ud;
            sp_pitchamdf_destroy(&pitchamdf);
            break;
        default:
            fprintf(stderr, "pitchamdf: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
