#include <string.h>
#include <stdlib.h>
#include "soundpipe.h"
#include "sporth.h"
#include "m_pd.h"

#define BUFSIZE 4096
#define BLKSIZE 64

static int sporth_pd_in(sporth_stack *stack, void *ud);
static int sporth_pdsend(plumber_data *pd, sporth_stack *stack, void **ud);
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

typedef struct {
    t_word *pdarray_vec;
    t_garray *pdarray;
    int pos;
    int size;
} pdsend;

static int add_ugens(plumber_data *pd, void *ud)
{
    plumber_ftmap_add_function(pd, "pdsend", sporth_pdsend, ud);
    return PLUMBER_OK;
}

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
            plumber_recompile_string_v2(&x->pd, x->buf1, x, add_ugens);
        } else {
            plumber_recompile_string_v2(&x->pd, x->buf0, x, add_ugens);
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
            post("String length too long!");
            return;
        }
        atom_string(ap, tmp, 250);
        len = strnlen(tmp, 250);
        for(c = 0; c < len; c++) {
            buf[bufpos] = tmp[c];
            bufpos++;
            if(bufpos > (BUFSIZE - 1)) {
                post("String length too long!");
                return;
            }
        }
        buf[bufpos++] = ' ';
    }
    buf[++bufpos] = 0;
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

static int sporth_pdsend(plumber_data *pd, sporth_stack *stack, void **ud)
{
    pdsend *ps;
    SPFLOAT in;
    const char *tab;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "fs") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            /* malloc and assign address to user data */
            ps = malloc(sizeof(pdsend));
            *ud = ps;
            tab = sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);

            ps->pdarray = 
                (t_garray *)pd_findbyclass(gensym(tab), garray_class);
            if(ps->pdarray != NULL) {
                garray_getfloatwords(ps->pdarray, &ps->size, &ps->pdarray_vec); 

                if(ps->size < BLKSIZE) {
                    post("sporth~: array %s should have size of %d or more\n",
                            tab, BLKSIZE);
                    stack->error++;
                    return PLUMBER_NOTOK;
                }
            } else {
                plumber_print(pd, "Could not find pd array %s\n", tab);
                stack->error++;
                return PLUMBER_NOTOK;
            }
            ps->pos = 0;
            break;
        case PLUMBER_INIT:
            sporth_stack_pop_string(stack);
            in = sporth_stack_pop_float(stack);
            break;

        case PLUMBER_COMPUTE:
            ps = *ud;

            in = sporth_stack_pop_float(stack);
            ps->pdarray_vec[ps->pos].w_float = in;
            ps->pos = (ps->pos + 1) % 64;
            break;

        case PLUMBER_DESTROY:
            ps = *ud;
            free(ps);
            break;
        default:
            fprintf(stderr, "gain: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
