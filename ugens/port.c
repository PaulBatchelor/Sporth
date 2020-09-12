#include "plumber.h"

int sporth_port(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    SPFLOAT smooth;
    SPFLOAT in = 0, out = 0;
    sp_smoother *data;
    switch(pd->mode){
        case PLUMBER_CREATE:
            sp_smoother_create(&data);
            plumber_add_ugen(pd, SPORTH_PORT, data);
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
               plumber_print(pd,"Not enough arguments for port\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            smooth = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            data = pd->last->ud;

            smooth = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            sp_smoother_init(pd->sp, data);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            data = pd->last->ud;

            smooth = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            data->smooth = smooth;

            sp_smoother_compute(pd->sp, data, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            data = pd->last->ud;
            sp_smoother_destroy(&data);
            break;
        default:
          plumber_print(pd,"Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}

int sporth_tport(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    SPFLOAT smooth;
    SPFLOAT in = 0, out = 0, trig = 0;
    sp_smoother *data;
    switch(pd->mode){
        case PLUMBER_CREATE:
            sp_smoother_create(&data);
            plumber_add_ugen(pd, SPORTH_TPORT, data);
            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
               plumber_print(pd,"Not enough arguments for port\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            smooth = sporth_stack_pop_float(stack);
            trig = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            data = pd->last->ud;

            smooth = sporth_stack_pop_float(stack);
            trig = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            sp_smoother_init(pd->sp, data);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            data = pd->last->ud;

            smooth = sporth_stack_pop_float(stack);
            trig = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);

            data->smooth = smooth;

            if(trig != 0) sp_smoother_reset(pd->sp, data, &in);
            sp_smoother_compute(pd->sp, data, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            data = pd->last->ud;
            sp_smoother_destroy(&data);
            break;
        default:
          plumber_print(pd,"Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}
