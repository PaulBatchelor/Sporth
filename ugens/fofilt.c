#include "plumber.h"

int sporth_fofilt(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT input;
    SPFLOAT out;
    SPFLOAT freq;
    SPFLOAT atk;
    SPFLOAT dec;
    sp_fofilt *fofilt;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "fofilt: Creating\n");
#endif

            sp_fofilt_create(&fofilt);
            plumber_add_ugen(pd, SPORTH_FOFILT, fofilt);
            if(sporth_check_args(stack, "ffff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for fofilt\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            dec = sporth_stack_pop_float(stack);
            atk = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "fofilt: Initialising\n");
#endif

            dec = sporth_stack_pop_float(stack);
            atk = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            fofilt = pd->last->ud;
            sp_fofilt_init(pd->sp, fofilt);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            dec = sporth_stack_pop_float(stack);
            atk = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            fofilt = pd->last->ud;
            fofilt->freq = freq;
            fofilt->atk = atk;
            fofilt->dec = dec;
            sp_fofilt_compute(pd->sp, fofilt, &input, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            fofilt = pd->last->ud;
            sp_fofilt_destroy(&fofilt);
            break;
        default:
            fprintf(stderr, "fofilt: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
