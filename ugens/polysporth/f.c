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


static pointer scm_eval(scheme *sc, pointer args);
static pointer scm_parse(scheme *sc, pointer args);
static pointer scm_turnon(scheme *sc, pointer args);
static pointer scm_turnoff(scheme *sc, pointer args);
static pointer scm_noteblock_begin(scheme *sc, pointer args);
static pointer scm_note(scheme *sc, pointer args);
static pointer scm_noteblock_end(scheme *sc, pointer args);
static pointer scm_metanote(scheme *sc, pointer args);
static pointer scm_set_release(scheme *sc, pointer args);
static pointer scm_lexer(scheme *sc, pointer args);
static pointer scm_init_sporthlet(scheme *sc, pointer args);
static pointer scm_show_pipes(scheme *sc, pointer args);
static pointer scm_write_code(scheme *sc, pointer args);
static pointer scm_set_callback(scheme *sc, pointer args);
static pointer scm_set_shutdown_callback(scheme *sc, pointer args);
static pointer scm_set_init_callback(scheme *sc, pointer args);
static pointer scm_rand(scheme *sc, pointer args);
static pointer scm_randi(scheme *sc, pointer args);
static pointer scm_ftbl(scheme *sc, pointer args);
static pointer scm_tset(scheme *sc, pointer args);
static pointer scm_tset(scheme *sc, pointer args);
static pointer scm_tget(scheme *sc, pointer args);
static pointer scm_mkvar(scheme *sc, pointer args);
static pointer scm_varset(scheme *sc, pointer args);
static pointer scm_varget(scheme *sc, pointer args);
static pointer scm_clear_events(scheme *sc, pointer args);
static pointer scm_gc_verbose(scheme *sc, pointer args);
static pointer scm_noteoff(scheme *sc, pointer args);
static pointer scm_argset(scheme *sc, pointer args);
static pointer scm_path(scheme *sc, pointer args);
static pointer scm_push_string(scheme *sc, pointer args);
static pointer scm_writecode(scheme *sc, pointer args);
static pointer scm_talias(scheme *sc, pointer args);
static pointer scm_pitch(scheme *sc, pointer args);
static pointer scm_noteoff_mode(scheme *sc, pointer args);
static pointer scm_import(scheme *sc, pointer args);
static pointer scm_export(scheme *sc, pointer args);
static pointer scm_mkftbl(scheme *sc, pointer args);
static pointer scm_bind_clock(scheme *sc, pointer args);
static pointer scm_note_delay(scheme *sc, pointer args);
static pointer scm_note_delay_add(scheme *sc, pointer args);
static pointer scm_note_delay_zero(scheme *sc, pointer args);

int ps_scm_load(polysporth *ps, const char *filename)
{
    /* load scheme */
    scheme *sc = &ps->sc;
    if(!scheme_init(sc)) {
        fprintf(stderr,"Polysporth: could not initialize!\n");
        return PLUMBER_NOTOK;
    }
    scheme_set_input_port_file(sc, stdin);
    scheme_set_output_port_file(sc, stdout);

    PS_FUNC("ps-eval", scm_eval);
    PS_FUNC("ps-parse", scm_parse);
    PS_FUNC("ps-turnon", scm_turnon);
    PS_FUNC("ps-turnoff", scm_turnoff);
    PS_FUNC("ps-init-sporthlet", scm_init_sporthlet);
    PS_FUNC("ps-lexer", scm_lexer);
    PS_FUNC("ps-write-code", scm_write_code);
    PS_FUNC("ps-show-pipes", scm_show_pipes);
    PS_FUNC("ps-noteblock-begin", scm_noteblock_begin);
    PS_FUNC("ps-clear-events", scm_clear_events);
    PS_FUNC("ps-noteblock-end", scm_noteblock_end);
    PS_FUNC("ps-set-release", scm_set_release);
    PS_FUNC("ps-note", scm_note);
    PS_FUNC("ps-metanote", scm_metanote);
    PS_FUNC("ps-set-callback", scm_set_callback);
    PS_FUNC("ps-set-init-callback", scm_set_init_callback);
    PS_FUNC("ps-set-shutdown-callback", scm_set_shutdown_callback);
    PS_FUNC("ps-rand", scm_rand);
    PS_FUNC("ps-randi", scm_randi);
    PS_FUNC("ps-ftbl", scm_ftbl);
    PS_FUNC("ps-tset", scm_tset);
    PS_FUNC("ps-tget", scm_tget);
    PS_FUNC("ps-mkvar", scm_mkvar);
    PS_FUNC("ps-varset", scm_varset);
    PS_FUNC("ps-varget", scm_varget);
    PS_FUNC("load-extension", scm_load_ext);
    PS_FUNC("ps-gc-verbose", scm_gc_verbose);
    PS_FUNC("ps-noteoff", scm_noteoff);
    PS_FUNC("ps-argset", scm_argset);
    PS_FUNC("ps-path", scm_path);
    PS_FUNC("ps-push-string", scm_push_string);
    PS_FUNC("ps-writecode", scm_writecode);
    PS_FUNC("ps-talias", scm_talias);
    PS_FUNC("ps-pitch", scm_pitch);
    PS_FUNC("ps-noteoff-mode", scm_noteoff_mode);
    PS_FUNC("ps-import", scm_import);
    PS_FUNC("ps-export", scm_export);
    PS_FUNC("ps-mkftbl", scm_mkftbl);
    PS_FUNC("ps-bind-clock", scm_bind_clock);
    PS_FUNC("ps-note-delay", scm_note_delay);
    PS_FUNC("ps-note-delay-zero", scm_note_delay_zero);
    PS_FUNC("ps-note-delay-add", scm_note_delay_add);

    /*
    scheme_define(sc,sc->global_env,mk_symbol(sc,"ps-path"),
            mk_string(sc, "/usr/local/share/sporth/polysporth/"));
    */
    sc->ext_data = (void *)ps;

    ps->cb = sc->NIL;
    ps->shutdown= sc->NIL;
    ps->init = sc->NIL;

    FILE *fp =fopen(filename,"r");
    if(fp == NULL) {
        return PLUMBER_NOTOK;
    }
    scheme_load_file(sc, fp);
    fclose(fp);

    /* rewind note events to beginning */
    dvector_rewind(&ps->events);
    return PLUMBER_OK;
}

static pointer scm_eval(scheme *sc, pointer args)
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
static pointer scm_parse(scheme *sc, pointer args)
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

static pointer scm_turnon(scheme *sc, pointer args)
{
    int id;
    int dur;
    polysporth *ps = sc->ext_data;
    id = (int) ivalue(car(args));
    args = cdr(args);

    if(is_number(car(args))) {
        dur = (int) ivalue(car(args));
    } else {
        /* -1 turns on note indefinitely */
        dur = -1;
    }

    ps_turnon_sporthlet(ps, id, dur);
    return sc->NIL;
}

static pointer scm_turnoff(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));
    ps_turnoff_sporthlet(ps, id);
    return sc->NIL;
}

static pointer scm_init_sporthlet(scheme *sc, pointer args)
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


static pointer scm_lexer(scheme *sc, pointer args)
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

static pointer scm_show_pipes(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));

    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_show_pipes(&ps->pd, &spl->pipes);
    return sc->NIL;
}

static pointer scm_write_code(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = (int) ivalue(car(args));
    /* TODO: error checking */
    sporthlet *spl = &ps->spl[id];
    plumbing_write_code(&ps->pd, &spl->pipes, stdout);
    return sc->NIL;
}

static pointer scm_noteblock_begin(scheme *sc, pointer args)
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

static pointer scm_noteblock_end(scheme *sc, pointer args)
{
#ifdef DEBUG_POLYSPORTH
    fprintf(stderr, "--- NOTEBLOCK END ---\n");
#endif
    polysporth *ps = sc->ext_data;
    if(ps->noteblock == PS_ON) {
        dvector_time_sort(&ps->tmp);
        dvector_time_to_delta(&ps->tmp);
        ps->noteblock = PS_OFF;
        if(ps->tmp.size > 0) {
            ps->events = dvector_merge(&ps->events, &ps->tmp);
            dvector_init(&ps->tmp);
        }
    }
    return sc->NIL;
}

static pointer scm_note(scheme *sc, pointer args)
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
    start += ps->delay;
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

static pointer scm_metanote(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int start = (int)ivalue(car(args));
    args = cdr(args);
    pointer func = car(args);
    dvector_append_metanote(&ps->tmp, start, func);
    return sc->NIL;
}

static pointer scm_set_release(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int release = ivalue(car(args));
    ps->reltime = release;
    return sc->NIL;
}

static pointer scm_set_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->cb = cb;
    return sc->NIL;
}

static pointer scm_set_init_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->init = cb;
    return sc->NIL;
}

static pointer scm_set_shutdown_callback(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    pointer cb = car(args);
    ps->shutdown = cb;
    return sc->NIL;
}

static pointer scm_rand(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    return mk_integer(sc, (long)sp_rand(ps->pd.sp));
}

static pointer scm_randi(scheme *sc, pointer args)
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

static pointer scm_ftbl(scheme *sc, pointer args)
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

static pointer scm_tset(scheme *sc, pointer args)
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

static pointer scm_tget(scheme *sc, pointer args)
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

static pointer scm_mkvar(scheme *sc, pointer args)
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

static pointer scm_varset(scheme *sc, pointer args)
{
    SPFLOAT *ptr;

    ptr = (SPFLOAT *)string_value(car(args));
    args = cdr(args);
    SPFLOAT val = rvalue(car(args));
    *ptr = val;
    return sc->NIL;
}

static pointer scm_varget(scheme *sc, pointer args)
{
    SPFLOAT *ptr;
    ptr = (SPFLOAT *)string_value(car(args));
    return mk_real(sc, *ptr);
}

static pointer scm_clear_events(scheme *sc, pointer args)
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

static pointer scm_gc_verbose(scheme *sc, pointer args)
{
    sc->gc_verbose = 1;
    return sc->NIL;
}

plumber_data * scheme_plumber(scheme *sc)
{
    polysporth *ps = sc->ext_data;
    return &ps->pd;
}

static pointer scm_noteoff(scheme *sc, pointer args)
{
    polysporth *ps = sc->ext_data;
    int id = ivalue(car(args));
    ps_sporthlet_noteoff(ps, id);
    return sc->NIL;
}

static pointer scm_argset(scheme *sc, pointer args)
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

static pointer scm_path(scheme *sc, pointer args)
{
    const char *path = getenv("POLYSPORTH_PLUGIN_PATH");
    if(path != NULL) 
        return mk_string(sc, path);
    else 
        return mk_string(sc, "/usr/local/share/sporth/polysporth");
}

static pointer scm_push_string(scheme *sc, pointer args)
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

static pointer scm_writecode(scheme *sc, pointer args)
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

static pointer scm_talias(scheme *sc, pointer args)
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

static pointer scm_pitch(scheme *sc, pointer args)
{
    const char *note;
    int nn;
    char nt;
    char ac;
    char oct;

    note = string_value(car(args));

    nn = 0;
    /* the note */
    nt = note[0];

    /* accidental -, #, or b */

    ac = note[1];

    /* octave */

    oct = note[2];

    switch(nt) {
        case 'C':
        case 'c':
            nn = 0;
            break;
        case 'd':
        case 'D':
            nn = 2;
            break;
        case 'e':
        case 'E':
            nn = 4;
            break;
        case 'f':
        case 'F':
            nn = 5;
            break;
        case 'g':
        case 'G':
            nn = 7;
            break;
        case 'a':
        case 'A':
            nn = 9;
            break;
        case 'b':
        case 'B':
            nn = 11;
            break;
    }

    switch(ac) {
        case 'b':
            nn -= 1;
            break;
        case '#':
            nn += 1;
            break;
    }

    switch(oct) {
        case '0':
            nn += 12;
            break;
        case '1':
            nn += 24;
            break;
        case '2':
            nn += 36;
            break;
        case '3':
            nn += 48;
            break;
        case '4':
            nn += 60;
            break;
        case '5':
            nn += 72;
            break;
        case '6':
            nn += 84;
            break;
        case '7':
            nn += 96;
            break;
        case '8':
            nn += 108;
            break;
        case '9':
            nn += 120;
            break;
    }

    return mk_real(sc, nn);
}

static pointer scm_noteoff_mode(scheme *sc, pointer args)
{
    int id;
    polysporth *ps;
    
    ps = sc->ext_data;
    id = ivalue(car(args));
  
    ps_sporthlet_mode_noteoff(ps, id);

    return sc->NIL;
}

static pointer scm_import(scheme *sc, pointer args)
{
    polysporth *ps;
    const char *ftname_src;
    const char *ftname_dst;
    plumber_data *pd_ext;
    plumber_data *pd;
    plumber_ftbl *ft_src;
    plumber_ftbl *ft_dst;

    ps = sc->ext_data;

    pd_ext = ps->pd_ext;
    pd = &ps->pd;

    ftname_src = string_value(car(args));
    args = cdr(args);
    ftname_dst = string_value(car(args));

    if(plumber_search(pd_ext, ftname_src, &ft_src) != PLUMBER_OK) {
        plumber_print(pd, "ps-import: Could not find ftable '%s'\n", ftname_src);
        return sc->NIL;
    }

    plumber_ftmap_delete(pd, 0);
    plumber_add(pd, ftname_dst, &ft_dst);
    ft_dst->type = ft_src->type;
    ft_dst->ud = ft_src->ud;
    plumber_ftmap_delete(pd, 1);

    return sc->NIL;
}

static pointer scm_export(scheme *sc, pointer args)
{
    polysporth *ps;
    const char *ftname_src;
    const char *ftname_dst;
    plumber_data *pd_ext;
    plumber_data *pd;
    plumber_ftbl *ft_src;
    plumber_ftbl *ft_dst;

    ps = sc->ext_data;

    /* opposite of scm_copy */
    /* TODO: refactor these functions */
    pd = ps->pd_ext;
    pd_ext = &ps->pd;

    ftname_src = string_value(car(args));
    args = cdr(args);
    ftname_dst = string_value(car(args));

    if(plumber_search(pd_ext, ftname_src, &ft_src) != PLUMBER_OK) {
        plumber_print(pd, "ps-export: Could not find ftable '%s'\n", ftname_src);
        return sc->NIL;
    }

    plumber_ftmap_delete(pd, 0);
    plumber_add(pd, ftname_dst, &ft_dst);
    ft_dst->type = ft_src->type;
    ft_dst->ud = ft_src->ud;
    plumber_ftmap_delete(pd, 1);

    return sc->NIL;
}

static pointer scm_mkftbl(scheme *sc, pointer args)
{
    const char *ftname;
    int size;
    sp_ftbl *ft;
    polysporth *ps;
    plumber_data *pd;

    ps = sc->ext_data;
    pd = &ps->pd;

    ftname = string_value(car(args));
    args = cdr(args);
    size = ivalue(car(args));

    sp_ftbl_create(pd->sp, &ft, size);

    plumber_ftmap_add(pd, ftname, ft);

    return mk_cptr(sc, (void **)&ft);
}

static pointer scm_bind_clock(scheme *sc, pointer args)
{
    polysporth *ps;
    const char *name;
    
    ps = sc->ext_data;
    name = string_value(car(args));

    if(ps_bind_clock(ps, name) != PLUMBER_OK) {
        plumber_print(&ps->pd, 
            "scm-bind-clock: could not find variable %s\n",
            name);
    }

    return sc->NIL;
}

static pointer scm_note_delay(scheme *sc, pointer args)
{
    polysporth *ps;
    ps = sc->ext_data;
    ps->delay = ivalue(car(args));
    return sc->NIL;
}

static pointer scm_note_delay_add(scheme *sc, pointer args)
{
    polysporth *ps;
    ps = sc->ext_data;
    ps->delay += ivalue(car(args));
    return sc->NIL;
}

static pointer scm_note_delay_zero(scheme *sc, pointer args)
{
    polysporth *ps;
    ps = sc->ext_data;
    ps->delay = 0;
    return sc->NIL;
}
