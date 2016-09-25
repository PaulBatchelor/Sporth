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


#include <stdlib.h>
#include <math.h>
#include <soundpipe.h>
#include <sporth.h>

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

static int bp2frq(plumber_data *pd, sporth_stack *stack, void **ud)
{
    SPFLOAT step, base;
    sporth_func_d *fd;
    bp_data *bd;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif

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

            bd = *ud;

            base = sporth_stack_pop_float(stack);
            step = sporth_stack_pop_float(stack);

            bd->base = base;

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            base = sporth_stack_pop_float(stack);
            step = sporth_stack_pop_float(stack);

            sporth_stack_push_float(stack, base * pow(3, 1.0 * step / 13));

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            bd = *ud;

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int main(int argc, char *argv[])
{
    plumber_data pd;
    sp_data *sp;
    sp_create(&sp);
    pd.sp = sp;
    plumber_register(&pd);
    plumber_init(&pd);
    
    char *sporth_string = "8 metro 14 0 count 250 _bp f 0.5 sine";

    bp_data bd; 
    plumber_ftmap_add_function(&pd, "bp", bp2frq, &bd);

    if(plumber_parse_string(&pd, sporth_string) == PLUMBER_OK) {
        plumber_compute(&pd, PLUMBER_INIT);
    } 
    
    sp_process(sp, &pd, process);

    plumber_clean(&pd);
    sp_destroy(&sp);

    return 0;
}
