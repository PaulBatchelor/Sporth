#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
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
static int ps_noteoff(plumber_data *pd, sporth_stack *stack, void **ud);

int ps_create(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl,
    char *out_tbl, char *filename)
{
    int i, j;

    /* search for input table */
    if(plumber_ftmap_search(pd, in_tbl, &ps->in) == PLUMBER_NOTOK) {
        stack->error++;
        return PLUMBER_NOTOK;
    }


    /* create output table */
    sp_ftbl_create(pd->sp, &ps->out, ninstances);
    plumber_ftmap_add(pd, out_tbl, ps->out);


    ps->nvoices = 0;

    plumber_register(&ps->pd);
    plumber_init(&ps->pd);
    ps->pd.sp = pd->sp;

    /* add input table to internal plumber instance with same name*/
    plumber_ftmap_delete(&ps->pd, 0);
    plumber_ftmap_add(&ps->pd, in_tbl, ps->in);
    plumber_ftmap_delete(&ps->pd, 1);

    /* create arg table */
    plumber_ftmap_delete(&ps->pd, 0);
    sp_ftbl_create(pd->sp, &ps->args, NARGS);
    plumber_ftmap_add(&ps->pd, "args", ps->args);
    plumber_ftmap_delete(&ps->pd, 1);

    /* create sporthlets */
    ps->spl = malloc(sizeof(sporthlet) * ninstances);

    /* PS_NULL ensures that the plumbing won't be compiled */
    for(i = 0; i < ninstances; i++) {
        ps->spl[i].state = PS_NULL;
        ps->spl[i].id = i;
        ps->spl[i].next = NULL;
        ps->spl[i].prev = NULL;
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

    plumber_ftmap_add_function(&ps->pd, "noteoff", ps_noteoff, ps);

    /* load scheme */
    ps_scm_load(ps, filename);

    return PLUMBER_OK;
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
    for(i = 0; i < ps->out->size; i++) {
        if(ps->spl[i].state != PS_NULL) {
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, "\tdestroying %d\n", i);
#endif
            plumbing_compute(&ps->pd, &ps->spl[i].pipes, PLUMBER_DESTROY);
            plumbing_destroy(&ps->spl[i].pipes);
        }
    }
    dvector_free(&ps->events);
    plumber_clean(&ps->pd);
    /* clean arg table stuff */
    sp_ftbl_destroy(&ps->args);
    scheme_load_string(&ps->sc, "(quit)");
    scheme_deinit(&ps->sc);
    free(ps->spl);
}

void ps_compute(polysporth *ps, SPFLOAT tick, SPFLOAT clock)
{
    SPFLOAT *out = ps->out->tbl;
    int i;
    int id;
    int count;
    dvalue *val;
    if(tick != 0) {
        if(ps->cb != ps->sc.NIL) scheme_call(&ps->sc, ps->cb, ps->sc.NIL);
    }

    if(clock != 0) {
        while(dvector_pop(&ps->events, &val)){
            if(val->type == PS_NOTE) {
#ifdef DEBUG_POLYSPORTH
                fprintf(stderr, "\t ### Time: %d Dur: %d\n",
                    ps->time, val->dur);
#endif
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
                if(val->nargs > 0) {
                    free(val->args);
                }
                free(val);
            } else if(val->type == PS_METANOTE) {
                scheme_call(&ps->sc, val->func, ps->sc.NIL);
                free(val);
            }
            if(ps->tmp.size > 0) {
                ps->events = dvector_merge(&ps->events, &ps->tmp);
                dvector_init(&ps->tmp);
            }
        }


        top_of_list(ps);
        count = get_voice_count(ps);
        sporthlet *spl, *next;
        spl = ps->root.next;
        for(i = 0; i < count; i++) {
            next = spl->next;
            id = spl->id;
            ps_decrement_clock(ps, id);
            spl = next;
        }
        ps->time++;
    }

    top_of_list(ps);
    count = get_voice_count(ps);
    SPFLOAT *tmp = ps->args->tbl;
    for(i = 0; i < count; i++) {
        id = get_next_voice_id(ps);
        ps->args->tbl = ps->spl[id].args;
        ps->id = id;
        out[id] = compute_sample(ps, id);
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
    if(spl->state == PS_ON) {
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
        if(ps->spl[i].state == PS_OFF || ps->spl[i].dur == 0) {
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
        ps_turnoff_sporthlet(ps, id);
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

static int ps_noteoff(plumber_data *pd, sporth_stack *stack, void **ud)
{
    SPFLOAT val;
    polysporth *ps;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            sporth_stack_pop_float(stack);
            break;
        case PLUMBER_INIT:
            val = sporth_stack_pop_float(stack);
            break;
        case PLUMBER_COMPUTE:
            val = sporth_stack_pop_float(stack);
            ps = *ud;
            if(val != 0) {
                ps_turnoff_sporthlet(ps, ps->id);
            }
            break;
        default:
            break;
    }
    return PLUMBER_OK;
}
