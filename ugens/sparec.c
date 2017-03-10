#include "plumber.h"

int sporth_sparec(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT input;
    const char * filename;
    sp_sparec *sparec;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            sp_sparec_create(&sparec);
            plumber_add_ugen(pd, SPORTH_SPAREC, sparec);
            if(sporth_check_args(stack, "fs") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for sparec\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            input = sporth_stack_pop_float(stack);
            break;
        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            input = sporth_stack_pop_float(stack);
            sparec = pd->last->ud;
            sp_sparec_init(pd->sp, sparec, filename);
            break;
        case PLUMBER_COMPUTE:
            input = sporth_stack_pop_float(stack);
            sparec = pd->last->ud;
            sp_sparec_compute(pd->sp, sparec, &input, NULL);
            break;
        case PLUMBER_DESTROY:
            sparec = pd->last->ud;
            sp_sparec_close(pd->sp, sparec);
            sp_sparec_destroy(&sparec);
            break;
    }
    return PLUMBER_OK;
}
