#include <stdlib.h>
#include "plumber.h"
#include "polysporth.h"

enum {
PS_ON,
PS_OFF,
PS_NULL,
PS_RELEASE,
PS_TRUE,
PS_FALSE,
PS_OK,
PS_NOTOK
};

static SPFLOAT compute_sample(polysporth *ps, int id);
static void ps_turnon_sporthlet(polysporth *ps, int id);
static void ps_turnoff_sporthlet(polysporth *ps, int id);

static void top_of_list(polysporth *ps);
static int get_next_voice_id(polysporth *ps);
static int get_voice_count(polysporth *ps);
static int is_first_element(polysporth *ps, int id);
static int is_last_element(polysporth *ps, int id);

static s7_pointer ps_eval(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_turnon(s7_scheme *sc, s7_pointer args);
static s7_pointer ps_turnoff(s7_scheme *sc, s7_pointer args);

int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename)
{
    int i;

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

    /* create sporthlets */
    ps->spl = malloc(sizeof(sporthlet) * ninstances);
   
    /* PS_NULL ensures that the plumbing won't be compiled */ 
    for(i = 0; i < ninstances; i++) {
        ps->spl[i].state = PS_NULL;
        ps->spl[i].id = i;
        ps->spl[i].next = NULL;
        ps->spl[i].prev = NULL;
        plumbing_init(&ps->spl[i].pipes);
    }

    /* set up linked list */
    ps->last = &ps->root;

    /* load scheme */
    ps->s7 = s7_init();
    s7_define_function(ps->s7, "ps-eval", ps_eval, 2, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-turnon", ps_turnon, 1, 0, false, "TODO");
    s7_define_function(ps->s7, "ps-turnoff", ps_turnoff, 1, 0, false, "TODO");
    s7_set_ud(ps->s7, (void *)ps);
    s7_load(ps->s7, filename);

    return PLUMBER_OK;
}

void ps_clean(polysporth *ps)
{
    int i;
    for(i = 0; i < ps->out->size; i++) {
        if(ps->spl[i].state != PS_NULL) {
            printf("destroying %d\n", i);
            plumbing_compute(&ps->pd, &ps->spl[i].pipes, PLUMBER_DESTROY);
            plumbing_destroy(&ps->spl[i].pipes);
        }
    }
    plumber_clean(&ps->pd);
    free(ps->spl);
}

void ps_compute(polysporth *ps, SPFLOAT tick)
{
    SPFLOAT *out = ps->out->tbl;
    if(tick != 0) {
        s7_call(ps->s7, s7_name_to_value(ps->s7, "run"), s7_nil(ps->s7));
    }
    int i;
    int id;
    top_of_list(ps);
    int count = get_voice_count(ps);
    for(i = 0; i < count; i++) {
        id = get_next_voice_id(ps);
        out[id] = compute_sample(ps, id);
    }

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

static SPFLOAT compute_sample(polysporth *ps, int id)
{
    SPFLOAT out = 0;
    sporthlet *spl = &ps->spl[id];
    plumber_data *pd = &ps->pd;
    
    if(spl->state != PS_NULL) {
        ps->pd.tmp = &spl->pipes;
        plumbing_compute(pd, &spl->pipes, PLUMBER_COMPUTE);
        out = sporth_stack_pop_float(&pd->sporth.stack);
    }

    return out;
}

static void ps_turnon_sporthlet(polysporth *ps, int id)
{
    sporthlet *spl = &ps->spl[id];
    if(spl->state == PS_OFF) {
        //ps->last->next = spl;
        //spl->prev = ps->last;
        //ps->last = spl;
        sporthlet *first = ps->root.next;
        if(ps->nvoices == 0) {
            ps->root.next = spl;
        } else {
            spl->next = first;
            first->prev = spl;
            ps->root.next = spl;
        }
        spl->state = PS_ON;
        ps->nvoices++;
    }
}

static void ps_turnoff_sporthlet(polysporth *ps, int id)
{
    sporthlet *spl = &ps->spl[id];
    if(spl->state == PS_ON) {
        sporthlet *prev = spl->prev;
        sporthlet *next = spl->next;

        if(is_first_element(ps, id)) {
            ps->root.next = next;
        } else if(is_last_element(ps, id)) {
            prev->next = NULL;
        } else {
            prev->next = next;
        }

        
        spl->next = NULL; 
        spl->prev = NULL;
        ps->nvoices--; 
    }
}

static s7_pointer ps_turnon(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    int id = s7_integer(s7_list_ref(sc, args, 0));
    ps_turnon_sporthlet(ps, id);
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
    return spl->id;
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
