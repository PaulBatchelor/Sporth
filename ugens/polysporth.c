#include <stdlib.h>
#include "plumber.h"

#ifndef NO_POLYSPORTH
#include "scheme-private.h"
#include "polysporth.h"
#endif

int sporth_ps(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
#ifdef NO_POLYSPORTH
    plumber_print(pd, "ps: Sorry, this version doesn't implement it\n");
    return PLUMBER_NOTOK;
#else
    polysporth *ps;

    const char *out_tbl;
    const char *filename;
    int ninstances;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "polysporth: Creating\n");
#endif

            ps = calloc(1, sizeof(polysporth));
            plumber_add_ugen(pd, SPORTH_POLYSPORTH, ps);
            if(sporth_check_args(stack, "fss") != SPORTH_OK) {
                plumber_print(pd, "polysporth: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            ps_create(pd, ps, ninstances);
            ps_setup_outtable(pd, ps, out_tbl);
            ps_setup_argtable(pd, ps, "args");

            if(ps_scm_load(ps, filename) != PLUMBER_OK) {
                plumber_print(pd, "Initialization of polysporth failed\n");
                return PLUMBER_NOTOK;
            }

            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
#ifdef DEBUG_MODE
            plumber_print(pd, "polysporth: Initialising\n");
#endif
            ps = pd->last->ud;
            ps_init(ps);
            break;

        case PLUMBER_COMPUTE:
            ninstances = (int) sporth_stack_pop_float(stack);
            ps = pd->last->ud;
            ps_compute(ps);

            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            ps_clean(ps);
            free(ps);
            break;
        default:
            break;
    }
    return PLUMBER_OK;
#endif
}

int sporth_psh(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
#ifdef NO_POLYSPORTH
    plumber_print(pd, "ps: Sorry, this version doesn't implement it\n");
    return PLUMBER_NOTOK;
#else
    polysporth *ps;

    const char *ftname;
    const char *filename;
    int ninstances;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            ps = calloc(1, sizeof(polysporth));
            plumber_add_ugen(pd, SPORTH_PSH, ps);
            if(sporth_check_args(stack, "sfs") != SPORTH_OK) {
                plumber_print(pd, "psh: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            ftname = sporth_stack_pop_string(stack);
            ps_create(pd, ps, ninstances);

            if(ps_scm_load(ps, filename) != PLUMBER_OK) {
                plumber_print(pd, "Initialization of polysporth failed\n");
                return PLUMBER_NOTOK;
            }

            ps_init(ps);
            plumber_ftmap_delete(pd, 0);
            plumber_ftmap_add_userdata(pd, ftname, ps);
            plumber_ftmap_delete(pd, 1);

            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            ftname = sporth_stack_pop_string(stack);
            break;

        case PLUMBER_COMPUTE:
            sporth_stack_pop_float(stack);
            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            ps_clean(ps);
            free(ps);
            break;
        default:
            break;
    }
    return PLUMBER_OK;
#endif
}

#ifndef NO_POLYSPORTH
static SPFLOAT compute_sample(polysporth *ps, SPFLOAT tog, int id)
{
    plumber_data *pd = &ps->pd;
    SPFLOAT out = 0;
    sporthlet *spl = &ps->spl[id];

    if(tog != 0 && spl->state != PS_ON) {
        spl->state = PS_ON;
        spl->pipes.tick = 1;
    } else if(tog == 0 && spl->state == PS_ON) {
        spl->state = PS_NOTEOFF;
    }

    if(spl->state == PS_ON || spl->state == PS_NOTEOFF) {
        ps->pd.tmp = &spl->pipes;
        ps->id = id;
        plumbing_compute(pd, &spl->pipes, PLUMBER_COMPUTE);
        out = sporth_stack_pop_float(&pd->sporth.stack);
    }

    if(spl->state == PS_PLEASE_SHUTUP) spl->state = PS_OFF;

    return out;
}
#endif

int sporth_pst(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
#ifdef NO_POLYSPORTH
    plumber_print(pd, "pst: Sorry, this isn't implemented here.\n");
    return PLUMBER_NOTOK;
#else
    polysporth **ps;
    const char *ftname;
    int id;
    SPFLOAT tog;
    SPFLOAT tmp;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            ps = malloc(sizeof(polysporth *));
            plumber_add_ugen(pd, SPORTH_PST, ps);
            if(sporth_check_args(stack, "ffs") != SPORTH_OK) {
                plumber_print(pd, "pst: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }
            ftname = sporth_stack_pop_string(stack);
            id = sporth_stack_pop_float(stack);
            tog = sporth_stack_pop_float(stack);

            id = plumber_ftmap_search_userdata(pd, ftname, (void **)ps); 
            if(id != PLUMBER_OK) { 
                plumber_print(pd, "pst: could not find table %s\n", ftname);
                return PLUMBER_NOTOK;
            }
            sporth_stack_push_float(stack, 0);
            break;

        case PLUMBER_INIT:
            ps = pd->last->ud;
            ftname = sporth_stack_pop_string(stack);
            id = sporth_stack_pop_float(stack);
            tog = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;

        case PLUMBER_COMPUTE:
            ps = pd->last->ud;
            id = sporth_stack_pop_float(stack);
            tog = sporth_stack_pop_float(stack);
            tmp = compute_sample(*ps, tog, id); 
            sporth_stack_push_float(stack, tmp);
            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            free(ps);
            break;
        default:
            break;
    }
    return PLUMBER_OK;
#endif
}

