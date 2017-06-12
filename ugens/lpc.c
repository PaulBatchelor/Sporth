#include "plumber.h"

int sporth_lpc(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT input;
    SPFLOAT output;
    int framesize;
    sp_lpc *lpc;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            sp_lpc_create(&lpc);
            plumber_add_ugen(pd, SPORTH_LPC, lpc);
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for lpc\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            framesize = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            framesize = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            lpc = pd->last->ud;
            sp_lpc_init(pd->sp, lpc, framesize);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            framesize = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            lpc = pd->last->ud;
            sp_lpc_compute(pd->sp, lpc, &input, &output);
            sporth_stack_push_float(stack, output);
            break;
        case PLUMBER_DESTROY:
            lpc = pd->last->ud;
            sp_lpc_destroy(&lpc);
            break;
    }
    return PLUMBER_OK;
}
