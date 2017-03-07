#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "scheme-private.h"
#include "plumber.h"
#include "polysporth.h"
#include "dynload.h"

#define car(p) ((p)->_object._cons._car)
#define cdr(p) ((p)->_object._cons._cdr)
#define PS_FUNC(NAME, FUNC) scheme_define(sc, sc->global_env, \
        mk_symbol(sc, NAME), \
        mk_foreign_func(sc, FUNC)) 


static pointer ps_eval(scheme *sc, pointer args);
static pointer ps_parse(scheme *sc, pointer args);
static pointer ps_turnon(scheme *sc, pointer args);
static pointer ps_turnoff(scheme *sc, pointer args);
static pointer ps_noteblock_begin(scheme *sc, pointer args);
static pointer ps_note(scheme *sc, pointer args);
static pointer ps_noteblock_end(scheme *sc, pointer args);
static pointer ps_metanote(scheme *sc, pointer args);
static pointer ps_set_release(scheme *sc, pointer args);
static pointer ps_lexer(scheme *sc, pointer args);
static pointer ps_init_sporthlet(scheme *sc, pointer args);
static pointer ps_show_pipes(scheme *sc, pointer args);
static pointer ps_write_code(scheme *sc, pointer args);
static pointer ps_set_callback(scheme *sc, pointer args);
static pointer ps_set_shutdown_callback(scheme *sc, pointer args);
static pointer ps_set_init_callback(scheme *sc, pointer args);
static pointer ps_rand(scheme *sc, pointer args);
static pointer ps_randi(scheme *sc, pointer args);
static pointer ps_ftbl(scheme *sc, pointer args);
static pointer ps_tset(scheme *sc, pointer args);
static pointer ps_tset(scheme *sc, pointer args);
static pointer ps_tget(scheme *sc, pointer args);
static pointer ps_mkvar(scheme *sc, pointer args);
static pointer ps_varset(scheme *sc, pointer args);
static pointer ps_varget(scheme *sc, pointer args);
static pointer ps_clear_events(scheme *sc, pointer args);
static pointer ps_gc_verbose(scheme *sc, pointer args);
static pointer ps_noteoff(scheme *sc, pointer args);
static pointer ps_argset(scheme *sc, pointer args);
static pointer ps_path(scheme *sc, pointer args);
static pointer ps_push_string(scheme *sc, pointer args);
static pointer ps_writecode(scheme *sc, pointer args);
static pointer ps_talias(scheme *sc, pointer args);

void ps_scm_load(polysporth *ps, char *filename)
{
    /* load scheme */
    scheme *sc = &ps->sc;
    if(!scheme_init(sc)) {
        fprintf(stderr,"Polysporth: could not initialize!\n");
        return;
    }
    sc->code = sc->NIL;
    scheme_set_input_port_file(sc, stdin);
    scheme_set_output_port_file(sc, stdout);

    PS_FUNC("ps-eval", ps_eval);
    PS_FUNC("ps-parse", ps_parse);
    PS_FUNC("ps-turnon", ps_turnon);
    PS_FUNC("ps-turnoff", ps_turnoff);
    PS_FUNC("ps-init-sporthlet", ps_init_sporthlet);
    PS_FUNC("ps-lexer", ps_lexer);
    PS_FUNC("ps-write-code", ps_write_code);
    PS_FUNC("ps-show-pipes", ps_show_pipes);
    PS_FUNC("ps-noteblock-begin", ps_noteblock_begin);
    PS_FUNC("ps-clear-events", ps_clear_events);
    PS_FUNC("ps-noteblock-end", ps_noteblock_end);
    PS_FUNC("ps-set-release", ps_set_release);
    PS_FUNC("ps-note", ps_note);
    PS_FUNC("ps-metanote", ps_metanote);
    PS_FUNC("ps-set-callback", ps_set_callback);
    PS_FUNC("ps-set-init-callback", ps_set_init_callback);
    PS_FUNC("ps-set-shutdown-callback", ps_set_shutdown_callback);
    PS_FUNC("ps-rand", ps_rand);
    PS_FUNC("ps-randi", ps_randi);
    PS_FUNC("ps-ftbl", ps_ftbl);
    PS_FUNC("ps-tset", ps_tset);
    PS_FUNC("ps-tget", ps_tget);
    PS_FUNC("ps-mkvar", ps_mkvar);
    PS_FUNC("ps-varset", ps_varset);
    PS_FUNC("ps-varget", ps_varget);
    PS_FUNC("load-extension", scm_load_ext);
    PS_FUNC("ps-gc-verbose", ps_gc_verbose);
    PS_FUNC("ps-noteoff", ps_noteoff);
    PS_FUNC("ps-argset", ps_argset);
    PS_FUNC("ps-path", ps_path);
    PS_FUNC("ps-push-string", ps_push_string);
    PS_FUNC("ps-writecode", ps_writecode);
    PS_FUNC("ps-talias", ps_talias);

    /*
    scheme_define(sc,sc->global_env,mk_symbol(sc,"ps-path"),
            mk_string(sc, "/usr/local/share/sporth/polysporth/"));
    */
    sc->ext_data = (void *)ps;

    ps->cb = sc->NIL;
    ps->shutdown= sc->NIL;
    ps->init = sc->NIL;

    FILE *fp =fopen(filename,"r");
    scheme_load_file(sc, fp);
    fclose(fp);
}

static pointer ps_eval(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    sporthlet *spl;
    plumber_data *pd = &ps->pd;
    int id = ivalue(car(args));
    args = cdr(args);
    const char *str = string_value(car(args));

    spl = &ps->spl[id];
    pd->tmp = &spl->pipes;
    plumbing_parse_string(pd, &spl->pipes, (char *)str);
    plumbing_compute(pd, &spl->pipes, PLUMBER_INIT);
    spl->state = PS_OFF;
    return sc->NIL;
}

/* Parse: just parses, doesn't call plumbing_compute. Can be called multiple times.
 * Designed to build up patches programatically. */
static pointer ps_parse(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    sporthlet *spl;
    plumber_data *pd = &ps->pd;

    int id = (int)ivalue(car(args));
    args = cdr(args);
    const char *str = string_value(car(args));
    spl = &ps->spl[id];
    pd->tmp = &spl->pipes;
    plumbing_parse_string(pd, &spl->pipes, (char *)str);
    spl->state = PS_OFF;
    return sc->NIL;
}

static pointer ps_turnon(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));
    args = cdr(args);
    int dur = (int) ivalue(car(args));
    ps_turnon_sporthlet(ps, id, dur);
    return sc->NIL;
}

static pointer ps_turnoff(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));
    ps_turnoff_sporthlet(ps, id);
    return sc->NIL;
}

static pointer ps_init_sporthlet(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int)ivalue(car(args));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_compute(&ps->pd, &spl->pipes, PLUMBER_INIT);
    sporth_stack_init(&ps->pd.sporth.stack);
    spl->state = PS_OFF;
    return sc->NIL;
}


static pointer ps_lexer(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int)ivalue(car(args));
    args = cdr(args);
    const char *str = string_value(car(args));
    uint32_t size = strlen(str);
    plumbing *old_tmp;
    /* TODO: error checking */
    sporthlet *spl= &ps->spl[id];
    int prev_mode = ps->pd.mode;
    ps->pd.mode = PLUMBER_CREATE;
    old_tmp = ps->pd.tmp; 
    ps->pd.tmp = &spl->pipes; 
    plumber_lexer(&ps->pd, &spl->pipes, (char *)str, size);
    ps->pd.tmp = old_tmp; 
    ps->pd.mode = prev_mode;
    return sc->NIL;
}

static pointer ps_show_pipes(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_show_pipes(&ps->pd, &spl->pipes);
    return sc->NIL;
}

static pointer ps_write_code(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));
    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_write_code(&ps->pd, &spl->pipes, stdout);
    return sc->NIL;
}

static pointer ps_noteblock_begin(scheme *sc, pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- NOTEBLOCK BEGIN ---\n");
#endif
    polysporth *ps = sc->ext_data;
    dvector_init(&ps->tmp);
    if(ps->noteblock == PS_OFF) {
        ps->noteblock = PS_ON;
    } else {
        fprintf(stderr,
            "Warning: noteblock already on. Not doing anything.\n");
    }
    return sc->NIL;
}

static pointer ps_noteblock_end(scheme *sc, pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- NOTEBLOCK END ---\n");
#endif
    polysporth *ps = sc->ext_data;
    if(ps->noteblock == PS_ON) {
        dvector_time_to_delta(&ps->tmp);
        ps->noteblock = PS_OFF;
        if(ps->tmp.size > 0) {
            ps->events = dvector_merge(&ps->events, &ps->tmp);
            dvector_init(&ps->tmp);
        }
    }
    return sc->NIL;
}

static pointer ps_note(scheme *sc, pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "ps-note: adding value\n");
#endif
    polysporth *ps = sc->ext_data;
    int grp_start = 
        (int)ivalue(car(args));
    args = cdr(args);
    int grp_end = 
        (int)ivalue(car(args));
    args = cdr(args);
    int start = 
        (int)ivalue(car(args));
    args = cdr(args);
    int dur = 
        (int)ivalue(car(args));
    args = cdr(args);
    /* add release value to duration */
    dur += ps->reltime;
    int len = 0;


    pointer tmp = args;
    while(args != sc->NIL) {
        len++;
#ifdef DEBUG_POLYSPORTH
        printf("value is %g\n", rvalue(car(args)));
#endif
        args = cdr(args);
    }

    SPFLOAT *argtbl = NULL;

    if(len > 0) {
        int i;
        argtbl = malloc(sizeof(SPFLOAT) * len);
        args = tmp;
        for(i = 0; i < len; i++) {
            argtbl[i] = rvalue(car(args));
            args = cdr(args);
#ifdef DEBUG_POLYSPORTH
            fprintf(stderr, "--- %g\n", argtbl[i]);
#endif
        }
    }

    dvector_append(&ps->tmp, grp_start, grp_end, start, dur, argtbl, len);
    return sc->NIL;
}

static pointer ps_metanote(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int start = (int)ivalue(car(args));
    args = cdr(args);
    pointer func = car(args);
    dvector_append_metanote(&ps->tmp, start, func);
    return sc->NIL;
}

static pointer ps_set_release(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int release = ivalue(car(args));
    ps->reltime = release;
    return sc->NIL;
}

static pointer ps_set_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->cb = cb;
    return sc->NIL;
}

static pointer ps_set_init_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->init = cb;
    return sc->NIL;
}

static pointer ps_set_shutdown_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->shutdown = cb;
    return sc->NIL;
}

static pointer ps_rand(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    return mk_integer(sc, (long)sp_rand(ps->pd.sp));
}

static pointer ps_randi(scheme *sc, pointer args)
{
    polysporth *ps;
    long min;
    long max;
    long out;

    ps = sc->ext_data;
    min = ivalue(car(args));
    args = cdr(args);
    max = ivalue(car(args));

    out = min + ((long)sp_rand(ps->pd.sp) % (max - min));
    return mk_integer(sc, out);
}

static pointer ps_ftbl(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    plumber_data *pd = &ps->pd;
    pointer out;
    sp_ftbl *ft;

    const char *str = string_value(car(args));
    if(plumber_ftmap_search(pd, str, &ft) != PLUMBER_OK) {
        fprintf(stderr, "Could not find ftable %s\n", str);
        pd->sporth.stack.error++;
    }

    out = mk_cptr(sc, (void **)&ft);
    return out;
}

static pointer ps_tset(scheme *sc, pointer args)
{
    sp_ftbl *ft;
    int index;
    SPFLOAT val;

    ft = (sp_ftbl *)string_value(car(args));
    args = cdr(args);
    index = ivalue(car(args));
    args = cdr(args);
    val = rvalue(car(args));
    ft->tbl[index] = val;
    return sc->NIL;
}

static pointer ps_tget(scheme *sc, pointer args)
{
    sp_ftbl *ft;
    int index;
    SPFLOAT val;

    ft = (sp_ftbl *)string_value(car(args));
    args = cdr(args);
    index = ivalue(car(args));
    args = cdr(args);
    val = ft->tbl[index];
    return mk_real(sc, val);
}

static pointer ps_mkvar(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    plumber_data *pd = &ps->pd;
    SPFLOAT *ptr;

    const char *name = string_value(car(args));
    args = cdr(args);
    SPFLOAT val = rvalue(car(args));
    plumber_create_var(pd, (char *)name, &ptr);
    *ptr = val;
    return mk_cptr(sc, (void **)&ptr);
}

static pointer ps_varset(scheme *sc, pointer args)
{
    SPFLOAT *ptr;

    ptr = (SPFLOAT *)string_value(car(args));
    args = cdr(args);
    SPFLOAT val = rvalue(car(args));
    *ptr = val;
    return sc->NIL;
}

static pointer ps_varget(scheme *sc, pointer args)
{
    SPFLOAT *ptr;
    ptr = (SPFLOAT *)string_value(car(args));
    return mk_real(sc, *ptr);
}

static pointer ps_clear_events(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    dvector_free(&ps->events);
    dvector_init(&ps->events);
    ps->nvoices = 0;
    int i;
    for(i = 0; i < ps->out->size; i++) {
        if(ps->spl[i].state != PS_NULL) {
            ps->spl[i].state = PS_OFF;
        }
    }
    return sc->NIL;
}

static pointer ps_gc_verbose(scheme *sc, pointer args)
{
    sc->gc_verbose = 1;
    return sc->NIL;
}

plumber_data * scheme_plumber(scheme *sc)
{
    polysporth *ps = sc->ext_data;
    return &ps->pd;
}

static pointer ps_noteoff(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = ivalue(car(args));
    ps_sporthlet_noteoff(ps, id);
    return sc->NIL;
}

static pointer ps_argset(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = ivalue(car(args));
    args = cdr(args);
    int pos = ivalue(car(args));
    args = cdr(args);
    SPFLOAT val = rvalue(car(args));
    ps_set_arg(ps, id, pos, val);
    return sc->NIL;
}

static pointer ps_path(scheme *sc, pointer args)
{
    const char *path = getenv("POLYSPORTH_PLUGIN_PATH");
    if(path != NULL) 
        return mk_string(sc, path);
    else 
        return mk_string(sc, "/usr/local/share/sporth/polysporth");
}

static pointer ps_push_string(scheme *sc, pointer args)
{
    /* plumber_add_string(&ps->pd, str); */
    polysporth *ps = sc->ext_data;
    char *tmp;
    int id = (int)ivalue(car(args));
    args = cdr(args);
    const char *str = string_value(car(args));
    sporthlet *spl= &ps->spl[id];
    tmp = plumber_add_string(&ps->pd, &spl->pipes, str);
    sporth_stack_push_string(&ps->pd.sporth.stack, &tmp);
    return sc->NIL;
}

static pointer ps_writecode(scheme *sc, pointer args)
{
    polysporth *ps;
    int id;
    const char *filename;
    FILE *fp;
    sporthlet *spl;

    ps = sc->ext_data;
    id = ivalue(car(args));
    args = cdr(args);
    filename = string_value(car(args));
    
    fp = fopen(filename, "w");
    spl = &ps->spl[id];
    plumbing_write_code(&ps->pd, &spl->pipes, fp);
    fclose(fp);
    return sc->NIL;
}

static pointer ps_talias(scheme *sc, pointer args)
{
    polysporth *ps;
    sp_ftbl *ft;
    const char *varname;
    const char *ftname;
    int index;
    SPFLOAT *var;
    plumber_data *pd;

    ps = sc->ext_data;
    pd = &ps->pd;
    varname = string_value(car(args));
    args = cdr(args);
    index = ivalue(car(args));
    args = cdr(args);
    ftname = string_value(car(args));
    args = cdr(args);

    if(plumber_ftmap_search(pd, ftname, &ft) == PLUMBER_NOTOK) {
        plumber_print(pd, "talias: could not find table '%s'\n", ftname);
        return sc->NIL;
    }
       
    var = &ft->tbl[index];

    plumber_ftmap_delete(pd, 0);
    plumber_ftmap_add_userdata(pd, varname, var);
    plumber_ftmap_delete(pd, 1);

    return mk_cptr(sc, (void **)&var);
}
