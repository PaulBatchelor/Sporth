#include <string.h>
#include "soundpipe.h"
#include "sporth.h"
#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#define BUFSIZE 4096

/* ------------------------ sporth~ ----------------------------- */

/* tilde object to take absolute value. */

static int sporth_pd_in(sporth_stack *stack, void *ud);
static t_class *sporth_class;

typedef struct _sporth
{
    t_object x_obj; 	/* obligatory header */
    t_float x_f;    	/* place to hold inlet's value if it's set by message */
    sp_data *sp;
    plumber_data pd;
    int please_parse;
    SPFLOAT in;
    int whichbuf;
    char buf0[BUFSIZE];
    char buf1[BUFSIZE];
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

    if(x->please_parse == 1) {
        x->please_parse = 0;
        plumber_recompile(&x->pd);
        fclose(x->pd.fp);
        x->pd.fp = NULL;
    } else if(x->please_parse == 2) {
        x->please_parse = 0;
        if(x->whichbuf) { 
            plumber_recompile_string(&x->pd, x->buf1);
        } else {
            plumber_recompile_string(&x->pd, x->buf0);
        }
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
    x->whichbuf = 0;
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

static void pprint (t_sporth *x, t_symbol *selector, int argcount, t_atom *argvec)
{
    int pos = (int)argvec[0].a_w.w_float % 16;
    post("p %d: %g", pos, x->pd.p[pos]);
}

static void parse (t_sporth *x, t_symbol *s, int ac, t_atom *av)
{
    char tmp[250];
    int i;
    char *buf;
    unsigned int bufpos;
    unsigned int len;
    unsigned int c;
    t_atom *ap;

    post("Parsing!, %d args\n", ac);
    bufpos = 0;

    if(x->whichbuf == 1) {
        buf = x->buf0;
        x->whichbuf = 0;
    } else {
        buf = x->buf1;
        x->whichbuf = 1;
    }

    for(ap = av, i = 0; i < ac; ap++, i++) {
        if(bufpos > (BUFSIZE - 1)) {
            post("Buffer overflow!");
            return;
        }
        atom_string(ap, tmp, 250);
        len = strnlen(tmp, 250);
        post("%s %d", tmp, len);
        for(c = 0; c < len; c++) {
            buf[bufpos] = tmp[c];
            bufpos++;
            if(bufpos > (BUFSIZE - 1)) {
                post("Buffer overflow!");
                return;
            }
        }
        buf[bufpos++] = ' ';
    }
    buf[++bufpos] = 0;
    post("%s %d\n", buf, bufpos);
    x->please_parse = 2;
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
    class_addmethod(sporth_class, (t_method)pprint, gensym("pprint"), A_GIMME, 0);
    class_addmethod(sporth_class, (t_method)parse, gensym("parse"), A_GIMME, 0);
}

static int sporth_pd_in(sporth_stack *stack, void *ud)
{
    plumber_data *pd = (plumber_data *) ud;

    t_sporth *data = (t_sporth *) pd->ud;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "PD IN: creating\n");
#endif
            fprintf(stderr, "PD IN: creating\n");
            plumber_add_ugen(pd, SPORTH_IN, NULL);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "PD IN: initialising.\n");
#endif
            fprintf(stderr, "PD IN: initialising.\n");

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            sporth_stack_push_float(stack, data->in);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: destroying.\n");
#endif

            break;

        default:
            fprintf(stderr, "Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
