#include <stdlib.h>
#include "plumber.h"
#include "s7.h"

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

typedef struct sporthlet {
    plumbing pipes;
    int release_timer;
    int main_timer;
    int state;
    sp_ftbl *argtbl;
    struct sporthlet *prev, *next;
} sporthlet;

typedef struct polysporth {
    s7_scheme *s7;
    plumber_data pd;
    sporthlet *spl;
    int nvoices;
    sp_ftbl *in;
    sp_ftbl *out;
    /* linked list */
    sporthlet root;
    sporthlet *last;
} polysporth;

static int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename);
static void ps_clean(polysporth *ps);
static void ps_compute(polysporth *ps, SPFLOAT tick);
static SPFLOAT compute_sample(polysporth *ps, int id);
static void ps_insert(polysporth *ps, int id, int duration, int release);
static void ps_remove(polysporth *ps, int id);

static void top_of_list(polysporth *ps);
static int get_next_voice_id(polysporth *ps);
static int get_time(polysporth *ps, int id);
static int get_voice_count(polysporth *ps);
static void decrement_timer(polysporth *ps, int id);
static int is_first_element(polysporth *ps);
static int is_last_element(polysporth *ps);

static s7_pointer ps_eval(s7_scheme *sc, s7_pointer args);

int sporth_polysporth(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    polysporth *ps;

    char *in_tbl;
    char *out_tbl;
    char *filename;
    int ninstances;
    SPFLOAT tick;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Creating\n");
#endif
            ps = malloc(sizeof(polysporth));
            plumber_add_ugen(pd, SPORTH_POLYSPORTH, ps);

            if(sporth_check_args(stack, "ffsss") != SPORTH_OK) {
                fprintf(stderr, "polysporth: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
          
            if(ps_init(pd, stack, ps, ninstances, in_tbl, out_tbl, filename) == PS_NOTOK) {
                fprintf(stderr, "Initialization of polysporth failed\n");
                free(filename);
                free(out_tbl);
                free(in_tbl);
                return PLUMBER_NOTOK;
            }

            free(filename);
            free(out_tbl);
            free(in_tbl);

            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            out_tbl = sporth_stack_pop_string(stack);
            in_tbl = sporth_stack_pop_string(stack);
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            
            free(filename);
            free(out_tbl);
            free(in_tbl);

#ifdef DEBUG_MODE
            fprintf(stderr, "polysporth: Initialising\n");
#endif
            break;

        case PLUMBER_COMPUTE:
            ps = pd->last->ud;
            ninstances = (int) sporth_stack_pop_float(stack);
            tick = sporth_stack_pop_float(stack);
            ps_compute(ps, tick);
            break;
        case PLUMBER_DESTROY:
            ps = pd->last->ud;
            ps_clean(ps);
            free(ps);
            break;
        default:
            fprintf(stderr, "polysporth: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

static int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename)
{
    int i;

    /* search for input table */
    if(plumber_ftmap_search(pd, in_tbl, &ps->in) == PLUMBER_NOTOK) {
        stack->error++;
        return PS_NOTOK;
    }

    /* create output table */
    sp_ftbl_create(pd->sp, &ps->out, ninstances);
    plumber_ftmap_add(pd, out_tbl, ps->out);

    ps->nvoices = 0;
   
    plumber_register(&ps->pd); 
    plumber_init(&ps->pd);
    ps->pd.sp = pd->sp;

    /* create sporthlets */
    ps->spl = malloc(sizeof(sporthlet) * ninstances);
   
    /* PS_NULL ensures that the plumbing won't be compiled */ 
    for(i = 0; i < ninstances; i++) {
        ps->spl[i].state = PS_NULL;
        plumbing_init(&ps->spl[i].pipes);
    }

    /* load scheme */
    ps->s7 = s7_init();
    s7_define_function(ps->s7, "ps-eval", ps_eval, 2, 0, false, "TODO");
    s7_set_ud(ps->s7, (void *)ps);
    s7_load(ps->s7, filename);

    return PS_OK;
}

static void ps_clean(polysporth *ps)
{
    int i;
    for(i = 0; i < ps->out->size; i++) {
        if(ps->spl[i].state != PS_NULL) {
            plumbing_destroy(&ps->spl[i].pipes);
        }
    }
    plumber_clean(&ps->pd);
    free(ps->spl);
}

static void ps_compute(polysporth *ps, SPFLOAT tick)
{
    SPFLOAT *out = ps->out->tbl;
    out[0] = compute_sample(ps, 0);
    out[1] = compute_sample(ps, 1);
}

static s7_pointer ps_eval(s7_scheme *sc, s7_pointer args)
{
    polysporth *ps = (polysporth *)s7_get_ud(sc);
    sporthlet *spl;
    plumber_data *pd = &ps->pd;

    int id = s7_integer(s7_list_ref(sc, args, 0));
    const char *str = s7_string(s7_list_ref(sc, args, 1));
    printf("\nid to render to %d\n", id);
    printf("sporth string: '%s'\n", str);
    spl = &ps->spl[id];
    pd->tmp = &spl->pipes;
    plumbing_parse_string(pd, &spl->pipes, (char *)str);
    plumbing_compute(pd, &spl->pipes, PLUMBER_INIT);
    spl->state = PS_OFF;
    return NULL;
    //return (s7_wrong_type_arg_error(sc, "sp_eval", 1, s7_car(args), "a blah"));
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
    } else {
        //fprintf(stderr, "PS_NULL UH OH!\n");
    }
    return out;
}
