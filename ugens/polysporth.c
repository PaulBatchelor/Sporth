#include <stdlib.h>
#include "plumber.h"

#ifdef BUILD_POLYSPORTH
#include "polysporth.h"
#endif

int sporth_polysporth(sporth_stack *stack, void *ud)
{
#ifdef BUILD_POLYSPORTH
    plumber_data *pd = ud;
    polysporth *ps;

    char *in_tbl;
    char *out_tbl;
    char *filename;
    int ninstances;
    SPFLOAT clock;
    SPFLOAT tick;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Creating\n");
#endif

            ps = malloc(sizeof(polysporth));
            plumber_add_ugen(pd, SPORTH_POLYSPORTH, ps);

            if(sporth_check_args(stack, "fffsss") != SPORTH_OK) {
                fprintf(stderr, "polysporth: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            clock = sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);

            if(ps_init(pd, stack, ps, ninstances, in_tbl, out_tbl, filename) == PLUMBER_NOTOK) {
                fprintf(stderr, "Initialization of polysporth failed\n");
                return PLUMBER_NOTOK;
            }

            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            clock = sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            
#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Initialising\n");
#endif
            break;

        case PLUMBER_COMPUTE:
            ninstances = (int) sporth_stack_pop_float(stack);
            clock = sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            ps = pd->last->ud;
            ps_compute(ps, tick, clock);

            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            ps_clean(ps);
            free(ps);
            break;
        default:
            fprintf(stderr, "polysporth: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
#else
    return PLUMBER_NOTOK;
#endif
}
