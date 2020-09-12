#include "plumber.h"

int sporth_dcblock(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    SPFLOAT out;
    SPFLOAT in;
    sp_dcblocker *data;
    switch(pd->mode){
        case PLUMBER_CREATE:
#ifdef DEBUG_MODE
            plumber_print(pd, "Creating module dcblk\n");
#endif
            sp_dcblocker_create(&data);
            plumber_add_ugen(pd, SPORTH_DCBLK, data);
            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                plumber_print(pd, "Not enough arguments for dcblk\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            data = pd->last->ud;
            in = sporth_stack_pop_float(stack);
            sp_dcblocker_init(pd->sp, data);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            data = pd->last->ud;
            in = sporth_stack_pop_float(stack);
            sp_dcblocker_compute(pd->sp, data, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            data = pd->last->ud;
            sp_dcblocker_destroy(&data);
            break;
        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}
