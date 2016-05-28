#include <stdio.h>
#include <math.h>
#include <soundpipe.h>
#include <sporth.h>


static int sporth_bp(plumber_data *pd, sporth_stack *stack, void **ud)
{
    int step = 0;
    SPFLOAT out = 0;
    SPFLOAT base = 0;
    switch(pd->mode){
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                stack->error++;
                fprintf(stderr,"Invalid arguments for bp.\n");
                return PLUMBER_NOTOK;
            }
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0.0);
            break;
        case PLUMBER_INIT:
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0.0);
            break;
        case PLUMBER_COMPUTE:
            base = sporth_stack_pop_float(stack);
            step = floor(sporth_stack_pop_float(stack));
            out = base * pow(3.0, step / 13.0);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            break;
        default:
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_bp;
}
