#include <stdlib.h>
#include "plumber.h"

int sporth_diskin(sporth_stack *stack, void *ud)
{
#ifndef NO_LIBSNDFILE
    plumber_data *pd = ud;
    SPFLOAT out;
    const char * filename;
    sp_diskin *diskin;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "diskin: Creating\n");
#endif

            sp_diskin_create(&diskin);
            plumber_add_ugen(pd, SPORTH_DISKIN, diskin);
            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for diskin\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "diskin: Initialising\n");
#endif

            filename = sporth_stack_pop_string(stack);
            diskin = pd->last->ud;
            sp_diskin_init(pd->sp, diskin, filename);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            diskin = pd->last->ud;
            sp_diskin_compute(pd->sp, diskin, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            diskin = pd->last->ud;
            sp_diskin_destroy(&diskin);
            break;
        default:
            plumber_print(pd, "diskin: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
#else 
    return PLUMBER_OK;
#endif
}
