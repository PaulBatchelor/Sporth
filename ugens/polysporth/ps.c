#include <stdlib.h>
#include <string.h>
#ifdef USE_DL
#include <dlfcn.h>
#endif
#include "plumber.h"
#include "scheme-private.h"
#include "polysporth.h"

#define NARGS 8

static SPFLOAT compute_sample(polysporth *ps, int id);
static void ps_decrement_clock(polysporth *ps, int id);

static void top_of_list(polysporth *ps);
static int get_next_voice_id(polysporth *ps);
static int find_free_voice(polysporth *ps, int grp_start, int grp_end);

static int get_voice_count(polysporth *ps);
static int is_first_element(polysporth *ps, int id);
static int is_last_element(polysporth *ps, int id);
static int ps_noteoff_ugen(plumber_data *pd, sporth_stack *stack, void **ud);
static int ps_offtick(plumber_data *pd, sporth_stack *stack, void **ud);

typedef struct {
    polysporth *ps;
    int prev_state;
} off_data;

int ps_create(plumber_data *pd, polysporth *ps, int ninstances)
{
    int i, j;

    ps->pd_ext = pd;
    ps->nvoices = 0;
    ps->ninstances = ninstances;

    plumber_register(&ps->pd);
    plumber_init(&ps->pd);
    ps->pd.sp = pd->sp;

    /* create sporthlets */
    ps->spl = malloc(sizeof(sporthlet) * ninstances);

    /* PS_NULL ensures that the plumbing won't be compiled */
    for(i = 0; i < ninstances; i++) {
        ps->spl[i].state = PS_NULL;
        ps->spl[i].id = i;
        ps->spl[i].next = NULL;
        ps->spl[i].prev = NULL;
        /* noteoff mode set to off */
        ps->spl[i].noteoff = 0;
        plumbing_init(&ps->spl[i].pipes);
        for(j = 0; j < NARGS; j++) ps->spl[i].args[j] = 0;
    }

    /* set up linked list */
    ps->last = &ps->root;

    /* initialize event table */
    dvector_init(&ps->events);
    dvector_init(&ps->tmp);


    /* set clock to 0 */
    ps->time = 0;

    /* set release time (in ticks) to 0 by default */
    ps->reltime = 0;

    /* set block boolean to off */
    ps->noteblock = PS_OFF;

    plumber_ftmap_add_function(&ps->pd, "noteoff", ps_noteoff_ugen, ps);
    plumber_ftmap_add_function(&ps->pd, "offtick", ps_offtick, ps);

    /* no arguments by default */

    ps->nargs = 0;

    /* set up libline */
    
    
    ps->lines = malloc(ll_lines_size());
    ll_lines_init(ps->lines, pd->sp->sr);
    ll_sporth_ugen(ps->lines, &ps->pd, "ll");

    /* initialize tick */

    ps->tick = 0;
    ps->ptick = &ps->tick;
    return PLUMBER_OK;
}

void ps_setup_outtable(plumber_data *pd, polysporth *ps, 
        const char *out_tbl)
{
    sp_ftbl_create(pd->sp, &ps->out, ps->ninstances);
    plumber_ftmap_add(pd, out_tbl, ps->out);
}

void ps_setup_argtable(plumber_data *pd, polysporth *ps, const char *name)
{
    plumber_ftmap_delete(&ps->pd, 0);
    /* set nargs to NARGS */
    ps->nargs = NARGS;
    sp_ftbl_create(pd->sp, &ps->args, NARGS);
    plumber_ftmap_add(&ps->pd, name, ps->args);
    plumber_ftmap_delete(&ps->pd, 1);
}

void ps_init(polysporth *ps)
{
    if(ps->init != ps->sc.NIL) {
        scheme_call(&ps->sc, ps->init, ps->sc.NIL);
    }
}

void ps_clean(polysporth *ps)
{
    if(ps->shutdown != ps->sc.NIL) {
        scheme_call(&ps->sc, ps->shutdown, ps->sc.NIL);
    }
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- PS CLEAN ---\n");
#endif
    int i;
    for(i = 0; i < ps->ninstances; i++) {
        if(ps->spl[i].state != PS_NULL) {
            plumbing_compute(&ps->pd, &ps->spl[i].pipes, PLUMBER_DESTROY);
            plumbing_destroy(&ps->spl[i].pipes);
        }
    }
    dvector_free(&ps->events);
    plumber_clean(&ps->pd);
    /* clean arg table stuff */
    if(ps->nargs != 0) sp_ftbl_destroy(&ps->args);
    scheme_load_string(&ps->sc, "(quit)");
    scheme_deinit(&ps->sc);
    free(ps->spl);

    /* clean up libline */

    ll_lines_free(ps->lines);
    free(ps->lines);
}

void ps_tick(polysporth *ps)
{
    int count;
    int i;
    int id;
    dvalue *val;
    sporthlet *spl, *next;
    while(dvector_pop(&ps->events, &val)){
        if(val->type == PS_NOTE) {
            id = find_free_voice(ps, val->grp_start, val->grp_end);
            if(id >= 0) {
                ps_turnon_sporthlet(ps, id, val->dur);
                int len = (val->nargs < NARGS) ? val->nargs : NARGS;
                for(i = 0; i < len; i++) {
                    ps->spl[id].args[i] = val->args[i];
                }
            } else {
                /* TODO: does this need to be handled better? */
                fprintf(stderr, "No free voices left!\n");
            }
            dvalue_free(&val);
        } else if(val->type == PS_METANOTE) {
            scheme_call(&ps->sc, val->func, ps->sc.NIL);
            dvalue_free(&val);
        }
        if(ps->tmp.size > 0) {
            ps->events = dvector_merge(&ps->events, &ps->tmp);
            dvector_init(&ps->tmp);
        }
    }


    top_of_list(ps);
    count = get_voice_count(ps);
    spl = ps->root.next;
    for(i = 0; i < count; i++) {
        next = spl->next;
        id = spl->id;
        ps_decrement_clock(ps, id);
        spl = next;
    }
    ps->time++;
}

void ps_compute(polysporth *ps)
{
    int count;
    int i;
    int id;
    SPFLOAT *out = ps->out->tbl;
    int do_shutup = 0;
    sporthlet *spl, *next;

    if(*ps->ptick != 0) {
        ps_tick(ps);
    }

    top_of_list(ps);
    count = get_voice_count(ps);
    SPFLOAT *tmp = ps->args->tbl;
    for(i = 0; i < count; i++) {
        id = get_next_voice_id(ps);
        ps->args->tbl = ps->spl[id].args;
        ps->id = id;
        out[id] = compute_sample(ps, id);
        if(ps->spl[id].state == PS_PLEASE_SHUTUP) {
            /*ps_turnoff_sporthlet(ps, id);*/
            do_shutup = 1;
        }
    }
  
    /* there has to be a better way... */
    /* top_of_list(ps); */
    spl = ps->root.next;
    if(do_shutup) {
        for(i = 0; i < count; i++) {
            next = spl->next;
            id = spl->id;
            if(ps->spl[id].state == PS_PLEASE_SHUTUP) {
                ps_turnoff_sporthlet(ps, id);
            }
            spl = next;
        }
    }
    ps->args->tbl = tmp;
}

static SPFLOAT compute_sample(polysporth *ps, int id)
{
    SPFLOAT out = 0;
    sporthlet *spl = &ps->spl[id];
    plumber_data *pd = &ps->pd;

    if(spl->state != PS_NULL || spl->state != PS_OFF) {
        ps->pd.tmp = &spl->pipes;
        plumbing_compute(pd, &spl->pipes, PLUMBER_COMPUTE);
        out = sporth_stack_pop_float(&pd->sporth.stack);
    }

    return out;
}

void ps_turnon_sporthlet(polysporth *ps, int id, int dur)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "ps_turnon: adding id %d\n", id);
#endif
    sporthlet *spl = &ps->spl[id];
    if(spl->state == PS_OFF) {
        sporthlet *first = ps->root.next;
        if(ps->nvoices == 0) {
            ps->root.next = spl;
        } else {
            spl->next = first;
            first->prev = spl;
            ps->root.next = spl;
        }
        spl->state = PS_ON;
        spl->pipes.tick = 1;
        spl->dur = dur;
        ps->nvoices++;
    }
}

void ps_turnoff_sporthlet(polysporth *ps, int id)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "ps_turnoff: removing id %d, state %d\n", id,
    ps->spl[id].state);
#endif
    sporthlet *spl = &ps->spl[id];
    if(spl->state == PS_ON || spl->state == PS_PLEASE_SHUTUP) {
        spl->state = PS_OFF;
        sporthlet *prev = spl->prev;
        sporthlet *next = spl->next;
        if(is_first_element(ps, id)) {
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, ">>> removing first element!\n");
#endif
            ps->root.next = next;
        } else if(is_last_element(ps, id)) {
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, ">>> removing last element!\n");
#endif
            prev->next = NULL;
        } else {
            prev->next = next;
            next->prev = prev;
        }
        ps->out->tbl[id] = 0;
        spl->next = NULL;
        spl->prev = NULL;
        ps->nvoices--;
    } else {
        fprintf(stderr, "this shouldn't happen\n");
        if(spl->state == PS_OFF) {
            fprintf(stderr, "this voice is already off...?\n");
        }
    }
}

static int get_voice_count(polysporth *ps)
{
    return ps->nvoices;
}

static void top_of_list(polysporth *ps)
{
    ps->last = &ps->root;
}

static int get_next_voice_id(polysporth *ps)
{
    sporthlet *spl = ps->last->next;
    ps->last = spl;
    /* TODO: see if things break if we remove this conditional */
    if(ps->nvoices == 1) {
        return ps->root.next->id;
    } else {
        return spl->id;
    }
}

static int find_free_voice(polysporth *ps, int grp_start, int grp_end)
{
    int i, tmp;
    if(grp_start > grp_end) {
        tmp = grp_start;
        grp_start = grp_end;
        grp_end = tmp;
    }
    for(i = grp_start; i <= grp_end; i++) {
        /* check if sporthlet is off or is about to be off */
        if(ps->spl[i].state == PS_OFF || 
            (ps->spl[i].dur == 0 && ps->spl[i].noteoff == 0)) {
            /* turn off sporthlet early */
            if(ps->spl[i].state == PS_ON) {
                ps_turnoff_sporthlet(ps, i);
            }
            return i;
        }
    }
    return -1;
}

static int is_first_element(polysporth *ps, int id)
{
    sporthlet *p1 = &ps->spl[id];
    sporthlet *p2 = ps->root.next;
    return p1 == p2;
}

static int is_last_element(polysporth *ps, int id)
{
    sporthlet *p1 = ps->spl[id].next;
    return p1 == NULL;
}

static void ps_decrement_clock(polysporth *ps, int id)
{
/*
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "ps_decrement: id is %d\n", id);
#endif
*/
    sporthlet *spl = &ps->spl[id];
    if(spl->dur < 0) {
        return;
    } else if(spl->dur == 0) {
        if(spl->noteoff == 1) {
            ps_sporthlet_noteoff(ps, id);
        } else {
            ps_turnoff_sporthlet(ps, id);
        }
    } else {
        spl->dur--;
    }
}

int polysporth_eval(plumber_ptr *p, const char *str)
{
    polysporth *ps = p->ud;
    scheme_load_string(&ps->sc, str);
    return PLUMBER_OK;
}

static int ps_noteoff_ugen(plumber_data *pd, sporth_stack *stack, void **ud)
{
    polysporth *ps;
    SPFLOAT val;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            ps = *ud;
            sporth_stack_pop_float(stack);
            break;
        case PLUMBER_INIT:
            sporth_stack_pop_float(stack);
            ps = *ud;
            break;
        case PLUMBER_COMPUTE:
            val = sporth_stack_pop_float(stack);
            ps = *ud;
            if(val != 0) {
                ps->spl[ps->id].state = PS_PLEASE_SHUTUP;
            }
            break;
        case PLUMBER_DESTROY:
            break;
    }

    return PLUMBER_OK;
}

static int ps_offtick(plumber_data *pd, sporth_stack *stack, void **ud)
{
    polysporth *ps;
    off_data *off;
    int state;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            ps = *ud;
            off = malloc(sizeof(off_data));
            off->ps = ps;
            off->prev_state = PS_NULL;
            *ud = off;
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            off = *ud;
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            off = *ud;
            state = off->ps->spl[off->ps->id].state;

            if(state == PS_NOTEOFF && off->prev_state != PS_NOTEOFF) {
                sporth_stack_push_float(stack, 1);
            } else {
                sporth_stack_push_float(stack, 0);
            }
            off->prev_state = state;

            break;
        case PLUMBER_DESTROY:
            off = *ud;
            free(off);
            break;
    }

    return PLUMBER_OK;
}

void ps_set_arg(polysporth *ps, int id, int pos, SPFLOAT val)
{
    ps->spl[id].args[pos] = val;
}

void ps_sporthlet_noteoff(polysporth *ps, int id)
{
    if(ps->spl[id].state != PS_OFF) {
        ps->spl[id].state = PS_NOTEOFF;
    }
}

void ps_sporthlet_mode_noteoff(polysporth *ps, int id)
{
    ps->spl[id].noteoff = 1;
}

int ps_bind_clock(polysporth *ps, const char *name)
{
    plumber_ftbl *ft;

    if(plumber_search(&ps->pd, name, &ft) != PLUMBER_OK) {
        return PLUMBER_NOTOK;
    }

    if(ft->type != PTYPE_USERDATA) return PLUMBER_NOTOK;

    ps->ptick = (SPFLOAT *)ft->ud;

    return PLUMBER_OK;
}
