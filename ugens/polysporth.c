#include <stdlib.h>
#include "plumber.h"
#include "s7.h"

enum {
PS_ON,
PS_OFF,
PS_RELEASE,
PS_TRUE,
PS_FALSE,
PS_OK,
PS_NOTOK
};

typedef struct sporthlet {
    plumbing *plumb;
    int release_timer;
    int main_timer;
    int state;
    sp_ftbl *argtbl;
    struct sporthlet *prev, *next;
} sporthlet;

typedef struct {
    s7_scheme *s7;
    plumber_data *pd;
    sporthlet *spl;
    int size;
    int nvoices;
    sp_ftbl *in;
    sp_ftbl *out;
    /* linked list */
    sporthlet root;
    sporthlet *last;
} polysporth;

static int ps_init(sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
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
          
            if(ps_init(stack, ps, ninstances, in_tbl, out_tbl, filename) == PS_NOTOK) {
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

static int ps_init(sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename)
{
    ps->s7 = s7_init();
    s7_load(ps->s7, filename);
    return PS_OK;
}

static void ps_clean(polysporth *ps)
{

}

static void ps_compute(polysporth *ps, SPFLOAT tick)
{

}
