#include "plumber.h"

int sporth_clip(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT in;
    SPFLOAT out;
    int meth;
    SPFLOAT lim;
    SPFLOAT arg;
    sp_clip *clip;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "clip: Creating\n");
#endif

            sp_clip_create(&clip);
            plumber_add_module(pd, SPORTH_CLIP, sizeof(sp_clip), clip);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "clip: Initialising\n");
#endif

            if(sporth_check_args(stack, "ffff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for clip\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            meth = sporth_stack_pop_float(stack);
            lim = sporth_stack_pop_float(stack);
            arg = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            clip = pd->last->ud;
            sp_clip_init(pd->sp, clip, meth, lim);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            if(sporth_check_args(stack, "ffff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for clip\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            meth = sporth_stack_pop_float(stack);
            lim = sporth_stack_pop_float(stack);
            arg = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            clip = pd->last->ud;
            clip->arg = arg;
            sp_clip_compute(pd->sp, clip, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            clip = pd->last->ud;
            sp_clip_destroy(&clip);
            break;
        default:
            fprintf(stderr, "clip: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
