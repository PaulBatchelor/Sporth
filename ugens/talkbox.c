#include "plumber.h"

int sporth_talkbox(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT source;
    SPFLOAT excitation;
    SPFLOAT out;
    SPFLOAT quality;
    sp_talkbox *talkbox;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            sp_talkbox_create(&talkbox);
            plumber_add_ugen(pd, SPORTH_TALKBOX, talkbox);
            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for talkbox\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            quality = sporth_stack_pop_float(stack);
            source = sporth_stack_pop_float(stack);
            excitation = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            quality = sporth_stack_pop_float(stack);
            source = sporth_stack_pop_float(stack);
            excitation = sporth_stack_pop_float(stack);
            talkbox = pd->last->ud;
            sp_talkbox_init(pd->sp, talkbox);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            quality = sporth_stack_pop_float(stack);
            excitation = sporth_stack_pop_float(stack);
            source = sporth_stack_pop_float(stack);
            talkbox = pd->last->ud;
            talkbox->quality = quality;
            sp_talkbox_compute(pd->sp, talkbox, &source, &excitation, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            talkbox = pd->last->ud;
            sp_talkbox_destroy(&talkbox);
            break;
    }
    return PLUMBER_OK;
}
