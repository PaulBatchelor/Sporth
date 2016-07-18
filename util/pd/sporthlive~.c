#include <stdlib.h>
#include <lo/lo.h>
#include "soundpipe.h"
#include "sporth.h"
#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif


/* ------------------------ sporth~ ----------------------------- */

/* tilde object to take absolute value. */

static int sporth_pd_in(sporth_stack *stack, void *ud);
static t_class *sporth_class;
static lo_server_thread st;

typedef struct _sporthlive
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    plumber_data pd;
    int please_parse;
    SPFLOAT in;
    char *str;
} t_sporth;

static int sporth_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    printf("Sporth string: %s\n", &argv[0]->s);

    t_sporth *ud = user_data;

    if(!ud->please_parse) {
        ud->str = strdup(&argv[0]->s); 
        ud->please_parse = 1;
    }

    fflush(stdout);

    return 0;
}

    /* this is the actual performance routine which acts on the samples.
    It's called with a single pointer "w" which is our location in the
    DSP call list.  We return a new "w" which will point to the next item
    after us.  Meanwhile, w[0] is just a pointer to dsp-perform itself
    (no use to us), w[1] and w[2] are the input and output vector locations,
    and w[3] is the number of points to calculate. */
static t_int *sporth_perform(t_int *w)
{
    t_sporth *x = (t_sporth *)(w[1]);
    t_float *in = (t_float *)(w[2]);
    t_float *out = (t_float *)(w[3]);
    int n = (int)(w[4]);

    if(x->please_parse) {
        printf("recompiling...\n");
        x->please_parse = 0;
        plumber_recompile_string(&x->pd, x->str);
        free(x->str);
    }

    while (n--)
    {
    	x->in = *(in++);
        plumber_compute(&x->pd, PLUMBER_COMPUTE);
        *out++ = (t_float) sporth_stack_pop_float(&x->pd.sporth.stack);
    }

    return (w+5);
}

    /* called to start DSP.  Here we call Pd back to add our perform
    routine to a linear callback list which Pd in turn calls to grind
    out the samples. */
static void sporth_dsp(t_sporth *x, t_signal **sp)
{
    dsp_add(sporth_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

static void *sporth_new(void)
{
    t_sporth *x = (t_sporth *)pd_new(sporth_class);
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_f = 0;
    sp_create(&x->sp);
    plumber_register(&x->pd);
    x->pd.sporth.flist[SPORTH_IN - SPORTH_FOFFSET].func = sporth_pd_in;
    plumber_init(&x->pd);
    x->pd.sp = x->sp;
    x->pd.ud = x;
    char *str = "0";
    x->please_parse = 0;
    plumber_parse_string(&x->pd, str);
    plumber_compute(&x->pd, PLUMBER_INIT);

    printf("starting sporthlive server\n");
    st = lo_server_thread_new("6449", error);
    lo_server_thread_add_method(st, "/sporth", "s", sporth_handler, x);
    lo_server_thread_start(st);

    return (x);
}

    /* this routine, which must have exactly this name (with the "~" replaced
    by "_tilde) is called when the code is first loaded, and tells Pd how
    to build the "class". */

static void sporth_free(t_sporth *x) 
{
    printf("freeing!\n");
    plumber_clean(&x->pd);
    sp_destroy(&x->sp);
}

static void pset (t_sporth *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    int pos = (int)argvec[0].a_w.w_float % 16;
    SPFLOAT val = (SPFLOAT)argvec[1].a_w.w_float;
    x->pd.p[pos] = val;
}

static void pprint (t_sporth *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    int pos = (int)argvec[0].a_w.w_float % 16;
    post("p %d: %g", pos, x->pd.p[pos]);
}


void sporthlive_tilde_setup(void)
{
    sporth_class = class_new(
            gensym("sporthlive~"), 
            (t_newmethod)sporth_new, 
            (t_method)sporth_free, 
    	    sizeof(t_sporth), 
            0, 
            A_DEFFLOAT, 
            0);

	    /* this is magic to declare that the leftmost, "main" inlet
	    takes signals; other signal inlets are done differently... */
    CLASS_MAINSIGNALIN(sporth_class, t_sporth, x_f);
    	/* here we tell Pd about the "dsp" method, which is called back
	when DSP is turned on. */
    class_addmethod(sporth_class, (t_method)sporth_dsp, gensym("dsp"), 0);

    class_addmethod(sporth_class, (t_method)pset, gensym("pset"), A_GIMME, 0);
    class_addmethod(sporth_class, (t_method)pprint, gensym("pprint"), A_GIMME, 0);
}

static int sporth_pd_in(sporth_stack *stack, void *ud)
{
    plumber_data *pd = (plumber_data *) ud;

    t_sporth *data = (t_sporth *) pd->ud;

    switch(pd->mode) {
        case PLUMBER_CREATE:
            fprintf(stderr, "PD IN: creating\n");
            plumber_add_ugen(pd, SPORTH_IN, NULL);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            fprintf(stderr, "PD IN: initialising.\n");

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            sporth_stack_push_float(stack, data->in);

            break;

        case PLUMBER_DESTROY:
            break;

        default:
            fprintf(stderr, "Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
