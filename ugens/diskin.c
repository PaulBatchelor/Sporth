#include "plumber.h"

int sporth_diskin(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;
    char * filename;
    sp_diskin *diskin;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "diskin: Creating\n");
#endif

            sp_diskin_create(&diskin);
            plumber_add_module(pd, SPORTH_DISKIN, sizeof(sp_diskin), diskin);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "diskin: Initialising\n");
#endif

            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for diskin\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            diskin = pd->last->ud;
            sp_diskin_init(pd->sp, diskin, filename);
            sporth_stack_push_float(stack, 0);
            free(filename);
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
            fprintf(stderr, "diskin: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
