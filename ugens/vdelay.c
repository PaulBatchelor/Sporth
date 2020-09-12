#include <stdlib.h>
#include "plumber.h"

int sporth_vdelay(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT in;
    SPFLOAT out;
    SPFLOAT maxdel;
    SPFLOAT feedback;
    SPFLOAT del;
    sp_vardelay *vd;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "vdelay: Creating\n");
#endif
            sp_vardelay_create(&vd);
            plumber_add_ugen(pd, SPORTH_VDELAY, vd);
            if(sporth_check_args(stack, "ffff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for vdelay\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            maxdel = sporth_stack_pop_float(stack);
            del = sporth_stack_pop_float(stack);
            feedback = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "vdelay: Initialising\n");
#endif

            maxdel = sporth_stack_pop_float(stack);
            del = sporth_stack_pop_float(stack);
            feedback = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            vd = pd->last->ud;
            sp_vardelay_init(pd->sp, vd, maxdel);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            vd = pd->last->ud;
            maxdel = sporth_stack_pop_float(stack);
            del = sporth_stack_pop_float(stack);
            feedback = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            vd->feedback = feedback;
            vd->del = del;
            sp_vardelay_compute(pd->sp, vd, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            vd = pd->last->ud;
            sp_vardelay_destroy(&vd);
            break;
        default:
            plumber_print(pd, "vdelay: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
