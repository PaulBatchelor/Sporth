#include <stdlib.h>
#include "plumber.h"
#include "polysporth.h"

int sporth_polysporth(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    polysporth *ps;

    char *in_tbl;
    char *out_tbl;
    char *filename;
    int ninstances;
    SPFLOAT tick;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Creating\n");
#endif
            ps = malloc(sizeof(polysporth));
            plumber_add_ugen(pd, SPORTH_POLYSPORTH, ps);

            if(sporth_check_args(stack, "ffsss") != SPORTH_OK) {
                fprintf(stderr, "polysporth: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
          
            if(ps_init(pd, stack, ps, ninstances, in_tbl, out_tbl, filename) == PLUMBER_NOTOK) {
                fprintf(stderr, "Initialization of polysporth failed\n");
                free(filename);
                free(out_tbl);
                free(in_tbl);
                return PLUMBER_NOTOK;
            }

            free(filename);
            free(out_tbl);
            free(in_tbl);

            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            
            free(filename);
            free(out_tbl);
            free(in_tbl);

#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Initialising\n");
#endif
            break;

        case PLUMBER_COMPUTE:
            ps = pd->last->ud;
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            ps_compute(ps, tick);
            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            ps_clean(ps);
            free(ps);
            break;
        default:
            fprintf(stderr, "polysporth: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
