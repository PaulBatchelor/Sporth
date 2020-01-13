#include "plumber.h"

int sporth_sndwarp(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;
    sp_ftbl *ft_samp;
    sp_ftbl *ft_win;
    int32_t maxoverlap;
    SPFLOAT amp;
    SPFLOAT timewarp;
    SPFLOAT resample;
    SPFLOAT begin;
    SPFLOAT wsize;
    SPFLOAT randw;
    SPFLOAT overlap;
    int timemode;
    sp_sndwarp *sndwarp;
    const char *samp_name;
    const char *win_name;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            sp_sndwarp_create(&sndwarp);
            plumber_add_ugen(pd, SPORTH_SNDWARP, sndwarp);
            if(sporth_check_args(stack, "fffffffffss") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for sndwarp\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            maxoverlap = sporth_stack_pop_float(stack);
            timemode = (int)sporth_stack_pop_float(stack);
            overlap = sporth_stack_pop_float(stack);
            randw = sporth_stack_pop_float(stack);
            wsize = sporth_stack_pop_float(stack);
            begin = sporth_stack_pop_float(stack);
            resample = sporth_stack_pop_float(stack);
            timewarp = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            samp_name = sporth_stack_pop_string(stack);
            win_name = sporth_stack_pop_string(stack);
            maxoverlap = sporth_stack_pop_float(stack);
            timemode = (int)sporth_stack_pop_float(stack);
            overlap = sporth_stack_pop_float(stack);
            randw = sporth_stack_pop_float(stack);
            wsize = sporth_stack_pop_float(stack);
            begin = sporth_stack_pop_float(stack);
            resample = sporth_stack_pop_float(stack);
            timewarp = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            sndwarp = pd->last->ud;

            if(plumber_ftmap_search(pd, samp_name, &ft_samp) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            if(plumber_ftmap_search(pd, win_name, &ft_win) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            sp_sndwarp_init(pd->sp, sndwarp, ft_samp, ft_win, maxoverlap);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            sporth_stack_pop_float(stack);
            timemode = (int)sporth_stack_pop_float(stack);
            overlap = sporth_stack_pop_float(stack);
            randw = sporth_stack_pop_float(stack);
            wsize = sporth_stack_pop_float(stack);
            begin = sporth_stack_pop_float(stack);
            resample = sporth_stack_pop_float(stack);
            timewarp = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            sndwarp = pd->last->ud;
            sndwarp->amp = amp;
            sndwarp->timewarp = timewarp;
            sndwarp->resample = resample;
            sndwarp->begin = begin;
            sndwarp->wsize = wsize;
            sndwarp->randw = randw;
            sndwarp->overlap = overlap;
            sndwarp->timemode = timemode;
            sp_sndwarp_compute(pd->sp, sndwarp, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            sndwarp = pd->last->ud;
            sp_sndwarp_destroy(&sndwarp);
            break;
    }
    return PLUMBER_OK;
}
