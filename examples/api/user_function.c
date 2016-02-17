/*
 * User Function example
 *
 * This C example creates a sporth CLI parse identical to the default one,
 * except with a BP scale generator defined for function slot 0 (called by running "0 f").
 *
 * To hear a simple implementation of this function in action, run:
 *
 * echo "8 metro 14 0 count 250 0 f 0.5 sine" | ./user_function
 *
 */



#include <math.h>
#include "plumber.h"

typedef struct {
    plumber_data pd;
} UserData;

typedef struct {
    SPFLOAT base;
} bp_data;

void process(sp_data *sp, void *ud)
{
    UserData *data = ud;
    plumber_data *pd = &data->pd;
    plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out;
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}

int bp2frq(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT step, base;
    sporth_func_d *fd;
    bp_data *bd;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif

            fd = pd->last->ud;
            fd->ud = malloc(sizeof(bp_data));

            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            bd = fd->ud;

            base = sporth_stack_pop_float(stack);
            step = sporth_stack_pop_float(stack);

            bd->base = base;

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            base = sporth_stack_pop_float(stack);
            step = sporth_stack_pop_float(stack);

            sporth_stack_push_float(stack, base * pow(3, 1.0 * step / 13));

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;
            bd = fd->ud;

            free(bd);

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int main(int argc, char *argv[])
{
    UserData ud;
    plumber_init(&ud.pd);
    ud.pd.f[0] = bp2frq;
    sporth_run(&ud.pd, argc, argv, &ud, process);
    return 0;
}
