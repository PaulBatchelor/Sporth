#include <stdlib.h>
#include <stdio.h>
#include "plumber.h"

int sporth_tabread(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;
    const char *ftname;
    sp_ftbl *ft;
    SPFLOAT index;
    SPFLOAT mode;
    SPFLOAT offset;
    SPFLOAT wrap;
    sp_tread *tread;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "tread: Creating\n");
#endif

            sp_tread_create(&tread);
            plumber_add_ugen(pd, SPORTH_TABREAD, tread);
            if(sporth_check_args(stack, "ffffs") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for tread\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            ftname = sporth_stack_pop_string(stack);
            wrap = sporth_stack_pop_float(stack);
            offset = sporth_stack_pop_float(stack);
            mode = sporth_stack_pop_float(stack);
            index = sporth_stack_pop_float(stack);

            if(plumber_ftmap_search(pd, ftname, &ft) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            sp_tread_init(pd->sp, tread, ft, mode);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "tread: Initialising\n");
#endif
            ftname = sporth_stack_pop_string(stack);
            wrap = sporth_stack_pop_float(stack);
            offset = sporth_stack_pop_float(stack);
            mode = sporth_stack_pop_float(stack);
            index = sporth_stack_pop_float(stack);
            tread = pd->last->ud;

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            wrap = sporth_stack_pop_float(stack);
            offset = sporth_stack_pop_float(stack);
            mode = sporth_stack_pop_float(stack);
            index = sporth_stack_pop_float(stack);
            tread = pd->last->ud;
            tread->index = index;
            tread->mode = mode;
            tread->offset = offset;
            tread->wrap = wrap;
            sp_tread_compute(pd->sp, tread, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            tread = pd->last->ud;
            sp_tread_destroy(&tread);
            break;
        default:
            plumber_print(pd, "tread: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
