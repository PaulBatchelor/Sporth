#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "plumber.h"
#include "polysporth.h"

#define NARGS 8

static SPFLOAT compute_sample(polysporth *ps, int id);
static void ps_turnon_sporthlet(polysporth *ps, int id, int dur);
static void ps_turnoff_sporthlet(polysporth *ps, int id);
static void ps_decrement_clock(polysporth *ps, int id);

static void top_of_list(polysporth *ps);
static int get_next_voice_id(polysporth *ps);
static int find_free_voice(polysporth *ps, int grp_start, int grp_end);

static int get_voice_count(polysporth *ps);
static int is_first_element(polysporth *ps, int id);
static int is_last_element(polysporth *ps, int id);

static s7_pointer ps_eval(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_parse(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_turnon(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_turnoff(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_noteblock_begin(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_note(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_noteblock_end(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_metanote(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_set_release(s7_scheme *sc, s7_pointer args);
static s7_pointer cload(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_tset(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_tget(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_lexer(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_init_sporthlet(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_show_pipes(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_write_code(s7_scheme *sc, s7_pointer args);
static void *library = NULL;

int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl,
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
    plumber_ftmap_add_userdata(&ps->pd, in_tbl, ps->in);
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


    /* set clock to 0 */
    ps->time = 0;

    /* set release time (in ticks) to 0 by default */
    ps->reltime = 0;

    /* set block boolean to off */
    ps->noteblock = PS_OFF;

    /* load scheme */
    ps->s7 = s7_init();
    s7_define_function(ps->s7, "ps-eval", ps_eval, 2, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-parse", ps_parse, 2, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-turnon", ps_turnon, 2, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-turnoff", ps_turnoff, 1, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-note", ps_note, 5, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-noteblock-begin", ps_noteblock_begin, 0, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-noteblock-end", ps_noteblock_end, 0, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-metanote", ps_metanote, 2, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-tset", ps_tset, 3, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-tget", ps_tget, 2, 1, false, "TODO");
    s7_define_function(ps->s7, "ps-set-release", ps_set_release, 1, 0, false, NULL);
    s7_define_function(ps->s7, "cload", cload, 2, 0, false, NULL);
    s7_define_function(ps->s7, "ps-lexer", ps_lexer, 2, 0, false, NULL);
    s7_define_function(ps->s7, "ps-init-sporthlet", ps_init_sporthlet, 1, 0, false, NULL);
    s7_define_function(ps->s7, "ps-show-pipes", ps_show_pipes, 1, 0, false, NULL);
    s7_define_function(ps->s7, "ps-write-code", ps_write_code, 1, 0, false, NULL);
    s7_set_ud(ps->s7, (void *)ps);
    s7_load(ps->s7, filename);

    return PLUMBER_OK;
}

void ps_clean(polysporth *ps)
{
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
        s7_call(ps->s7, s7_name_to_value(ps->s7, "run"), s7_nil(ps->s7));
    }

    if(clock != 0) {
        if(ps->tmp.size > 0) {
            ps->events = dvector_merge(&ps->events, &ps->tmp);
            dvector_init(&ps->tmp);
        }
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
                s7_call(ps->s7, val->func, s7_nil(ps->s7));
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
        out[id] = compute_sample(ps, id);
    }
    ps->args->tbl = tmp;

}

static s7_pointer ps_eval(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    sporthlet *spl;
    plumber_data *pd = &ps->pd;

    int id = s7_integer(s7_list_ref(sc, args, 0));
    const char *str = s7_string(s7_list_ref(sc, args, 1));
    spl = &ps->spl[id];
    pd->tmp = &spl->pipes;
    plumbing_parse_string(pd, &spl->pipes, (char *)str);
    plumbing_compute(pd, &spl->pipes, PLUMBER_INIT);
    spl->state = PS_OFF;
    return NULL;
}

/* Parse: just parses, doesn't call plumbing_compute. Can be called multiple times.
 * Designed to build up patches programatically. */
static s7_pointer ps_parse(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    sporthlet *spl;
    plumber_data *pd = &ps->pd;

    int id = s7_integer(s7_list_ref(sc, args, 0));
    const char *str = s7_string(s7_list_ref(sc, args, 1));
    spl = &ps->spl[id];
    pd->tmp = &spl->pipes;
    plumbing_parse_string(pd, &spl->pipes, (char *)str);
    spl->state = PS_OFF;
    return NULL;
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

static void ps_turnon_sporthlet(polysporth *ps, int id, int dur)
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

static void ps_turnoff_sporthlet(polysporth *ps, int id)
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

static s7_pointer ps_turnon(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));
    int dur = s7_integer(s7_list_ref(sc, args, 1));
    ps_turnon_sporthlet(ps, id, dur);
    return NULL;
}

static s7_pointer ps_turnoff(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));
    ps_turnoff_sporthlet(ps, id);
    return NULL;
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

static s7_pointer ps_noteblock_begin(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- NOTEBLOCK BEGIN ---\n");
#endif
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    dvector_init(&ps->tmp);
    if(ps->noteblock == PS_OFF) {
        ps->noteblock = PS_ON;
    } else {
        fprintf(stderr,
            "Warning: noteblock already on. Not doing anything.\n");
    }
    return NULL;
}

static s7_pointer ps_note(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, "ps-note: adding value\n");
#endif
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int grp_start = s7_integer(s7_list_ref(sc, args, 0));
    int grp_end = s7_integer(s7_list_ref(sc, args, 1));
    int start = s7_integer(s7_list_ref(sc, args, 2));
    int dur = s7_integer(s7_list_ref(sc, args, 3));
    /* add release value to duration */
    dur += ps->reltime;
    int len = s7_list_length(sc, (s7_list_ref(sc, args, 4)));
    SPFLOAT *argtbl = NULL;
    s7_pointer tbl = s7_list_ref(sc, args, 4);
    s7_pointer val;
    if(len > 0) {
        int i;
        argtbl = malloc(sizeof(SPFLOAT) * len);
        for(i = 0; i < len; i++) {
            val = s7_list_ref(sc, tbl, i);
            if(s7_is_integer(val)) {
                argtbl[i] = (SPFLOAT) s7_integer(val);
            } else {
                argtbl[i] = (SPFLOAT) s7_real(val);
            }
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, "--- %g\n", argtbl[i]);
#endif
        }
    }
    dvector_append(&ps->tmp, grp_start, grp_end, start, dur, argtbl, len);
    return NULL;
}

static s7_pointer ps_metanote(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int start = s7_integer(s7_list_ref(sc, args, 0));
    s7_pointer func = s7_list_ref(sc, args, 1);
    dvector_append_metanote(&ps->tmp, start, func);
    return NULL;
}

static s7_pointer ps_noteblock_end(s7_scheme *sc, s7_pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- NOTEBLOCK END ---\n");
#endif
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    if(ps->noteblock == PS_ON) {
        dvector_time_to_delta(&ps->tmp);
        //ps->events = dvector_merge(&ps->events, &ps->tmp);
        ps->noteblock = PS_OFF;
    }
    return NULL;
}

static s7_pointer ps_tset(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    sp_ftbl *ft;
    /*TODO: add type checking */
    const char *str = s7_string(s7_list_ref(sc, args, 0));
    int pos = s7_integer(s7_list_ref(sc, args, 1));
    s7_double val = s7_real(s7_list_ref(sc, args, 2));

    if(plumber_ftmap_search(&ps->pd, str, &ft) == PLUMBER_NOTOK) {
        fprintf(stderr, "Could not find table %s\n", str);
        return s7_make_real(sc, 0.0);
    }

    if(pos > ft->size) {
        fprintf(stderr, "Warning: tset: position %d out of bounds.\n",
            pos);
        return s7_nil(sc);
    }

    ft->tbl[pos] = (SPFLOAT) val;

    return s7_nil(sc);
}

static s7_pointer ps_tget(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    sp_ftbl *ft;
    const char *str = s7_string(s7_list_ref(sc, args, 0));
    int pos = s7_integer(s7_list_ref(sc, args, 1));

    if(plumber_ftmap_search(&ps->pd, str, &ft) == PLUMBER_NOTOK) {
        return s7_make_real(sc, 0.0);
    }

    if(pos > ft->size) {
        fprintf(stderr, "Warning: tset: position %d out of bounds.\n",
            pos);
        return s7_make_real(sc, 0.0);
    }

    return s7_make_real(sc, ft->tbl[pos]);
}

static s7_pointer ps_set_release(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int release = s7_integer(s7_list_ref(sc, args, 0));
    ps->reltime = release;
    return s7_nil(sc);
}

static s7_pointer cload(s7_scheme *sc, s7_pointer args)
{
  /* cload loads a shared library */
  //#define CLOAD_HELP "(cload so-file-name) loads the module"
  library = dlopen(s7_string(s7_car(args)), RTLD_LAZY);
  if (library)
    {
      /* call our init func to define add-1 in s7 */
      void *init_func;
      init_func = dlsym(library, s7_string(s7_cadr(args)));
      if (init_func)
	{
	  typedef void *(*dl_func)(s7_scheme *sc);
	  ((dl_func)init_func)(sc);  /* call the initialization function (init_ex above) */
	  return(s7_t(sc));
	}
    }
  return(s7_error(sc, s7_make_symbol(sc, "load-error"),
		      s7_list(sc, 2, s7_make_string(sc, "loader error: ~S"),
			             s7_make_string(sc, dlerror()))));
}

static s7_pointer ps_lexer(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));
    const char *str = s7_string(s7_list_ref(sc, args, 1));
    uint32_t size = strlen(str);
    plumbing *old_tmp;
    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    int prev_mode = ps->pd.mode;
    ps->pd.mode = PLUMBER_CREATE;
    old_tmp = ps->pd.tmp; 
    ps->pd.tmp = &spl->pipes; 
    plumber_lexer(&ps->pd, &spl->pipes, (char *)str, size);
    ps->pd.tmp = old_tmp; 
    ps->pd.mode = prev_mode;
    return s7_nil(sc);
}

static s7_pointer ps_init_sporthlet(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_compute(&ps->pd, &spl->pipes, PLUMBER_INIT);
    spl->state = PS_OFF;
    return s7_nil(sc);
}

static s7_pointer ps_show_pipes(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_show_pipes(&ps->pd, &spl->pipes);
    return s7_nil(sc);
}
static s7_pointer ps_write_code(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_write_code(&ps->pd, &spl->pipes, stdout);
    return s7_nil(sc);
}
