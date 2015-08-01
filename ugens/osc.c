#include "plumber.h"
#include "macros.h"

typedef struct {
    sp_fosc *osc;
    sp_ftbl *ft;
} sporth_fm_d;

int sporth_osc(sporth_stack *stack, void *ud) 
{
    if(stack->error > 0) return PLUMBER_NOTOK;

    plumber_data *pd = ud;
    SPFLOAT out = 0, amp, freq;
    char *ftname;
    sp_ftbl *ft;
    sp_osc *osc;
    switch(pd->mode){
        case PLUMBER_CREATE:
            printf("creating FM function... \n");
            sp_osc_create(&osc);
            plumber_add_module(pd, SPORTH_OSC, sizeof(sp_osc), osc);
            break;
        case PLUMBER_INIT:
            if(sporth_check_args(stack, "ffs") != SPORTH_OK) {
                stack->error++;
                printf("Invalid arguments for osc.\n");
                return PLUMBER_NOTOK;
            }
            osc = pd->last->ud;

            ftname = sporth_stack_pop_string(stack);
            amp = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);

            if(plumber_ftmap_search(pd, ftname, &ft) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            sp_osc_init(pd->sp, osc, ft);
            sporth_stack_push_float(stack, 0.0);
            free(ftname);
            break;

        case PLUMBER_COMPUTE:
            osc = pd->last->ud;
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            amp = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);

            //osc->freq = freq;
            //osc->amp = amp;

            sp_osc_compute(pd->sp, osc, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            osc = pd->last->ud;
            sp_osc_destroy(&osc);
            break;
        default:
            printf("Error: Unknown mode!"); 
            stack->error++;
            return PLUMBER_NOTOK;
            break;
    }   
    return PLUMBER_OK;
}
