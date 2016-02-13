#include <soundpipe.h>
#include <sporth.h>
#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ------------------------ sporth~ ----------------------------- */

/* tilde object to take absolute value. */

static t_class *sporth_class;

typedef struct _sporth
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    plumber_data pd;
    int please_parse;
} t_sporth;

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
        x->please_parse = 0;
        plumber_recompile(&x->pd);
        fclose(x->pd.fp);
    }

    while (n--)
    {
    	float f = *(in++);
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
    printf("creating!\n");
    t_sporth *x = (t_sporth *)pd_new(sporth_class);
    outlet_new(&x->x_obj, gensym("signal"));
    x->x_f = 0;
    sp_create(&x->sp);
    plumber_register(&x->pd);
    plumber_init(&x->pd);
    x->pd.sp = x->sp;
    char *str = "0";
    x->please_parse = 0;

    plumber_parse_string(&x->pd, str);
    plumber_compute(&x->pd, PLUMBER_INIT);

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

static void open_file (t_sporth *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    post("Parsing file %s...", argvec[0].a_w.w_symbol->s_name);
    x->pd.fp = fopen(argvec[0].a_w.w_symbol->s_name, "r");
    x->please_parse = 1;
}

static void pset (t_sporth *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    int pos = (int)argvec[0].a_w.w_float % 16;
    SPFLOAT val = (SPFLOAT)argvec[1].a_w.w_float;
    x->pd.p[pos] = val;
}

void sporth_tilde_setup(void)
{
    sporth_class = class_new(
            gensym("sporth~"), 
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

    class_addmethod(sporth_class, (t_method)open_file, gensym("open"), A_GIMME, 0);
    class_addmethod(sporth_class, (t_method)pset, gensym("pset"), A_GIMME, 0);
}
