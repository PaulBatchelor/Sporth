#include "plumber.h"

int sporth_spa(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;
    const char * filename;
    sp_spa *spa;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            sp_spa_create(&spa);
            plumber_add_ugen(pd, SPORTH_SPA, spa);
            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for spa\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            spa = pd->last->ud;
            if(sp_spa_init(pd->sp, spa, filename) != SP_OK) {
                plumber_print(pd, "spa: could not open file %s\n", filename);
                return PLUMBER_NOTOK;
            }
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            spa = pd->last->ud;
            sp_spa_compute(pd->sp, spa, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            spa = pd->last->ud;
            sp_spa_destroy(&spa);
            break;
    }
    return PLUMBER_OK;
}
