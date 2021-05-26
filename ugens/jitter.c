#include "plumber.h"

typedef struct sp_jitter{
    SPFLOAT amp, cpsMin, cpsMax;
    SPFLOAT cps;
    int32_t phs;
    int initflag;
    SPFLOAT num1, num2, dfdmax;
} sp_jitter;

int sp_jitter_create(sp_jitter **p);
int sp_jitter_destroy(sp_jitter **p);
int sp_jitter_init(sp_data *sp, sp_jitter *p);
int sp_jitter_compute(sp_data *sp, sp_jitter *p, SPFLOAT *in, SPFLOAT *out);

static SPFLOAT sp_jitter_rand(sp_data *sp)
{
    SPFLOAT out = (SPFLOAT) ((sp_rand(sp) >> 1) & 0x7fffffff) *
    (4.656612875245796924105750827168e-10);
    return out;
}

static SPFLOAT sp_jitter_birand(sp_data *sp)
{
    SPFLOAT out = (SPFLOAT) (sp_rand(sp) & 0x7fffffff) *
    (4.656612875245796924105750827168e-10);
    return out;
}

int sp_jitter_create(sp_jitter **p)
{
    *p = malloc(sizeof(sp_jitter));
    return SP_OK;
}

int sp_jitter_destroy(sp_jitter **p)
{
    free(*p);
    return SP_OK;
}

int sp_jitter_init(sp_data *sp, sp_jitter *p)
{
    p->amp = 0.5;
    p->cpsMin = 0.5;
    p->cpsMax = 4;
    p->num2 = sp_jitter_birand(sp);
    p->initflag = 1;
    p->phs=0;
    return SP_OK;
}

int sp_jitter_compute(sp_data *sp, sp_jitter *p, SPFLOAT *in, SPFLOAT *out)
{
    if (p->initflag) {
      p->initflag = 0;
      *out = p->num2 * p->amp;
      p->cps = sp_jitter_rand(sp) * (p->cpsMax - p->cpsMin) + p->cpsMin;
      p->phs &= SP_FT_PHMASK;
      p->num1 = p->num2;
      p->num2 = sp_jitter_birand(sp);
      p->dfdmax = 1.0 * (p->num2 - p->num1) / SP_FT_MAXLEN;
      return SP_OK;
    }

    *out = (p->num1 + (SPFLOAT)p->phs * p->dfdmax) * p->amp;
    p->phs += (int32_t)(p->cps * (SPFLOAT)(SP_FT_MAXLEN / sp->sr));

    if (p->phs >= SP_FT_MAXLEN) {
      p->cps   = sp_jitter_rand(sp) * (p->cpsMax - p->cpsMin) + p->cpsMin;
      p->phs   &= SP_FT_PHMASK;
      p->num1   = p->num2;
      p->num2 =  sp_jitter_birand(sp);
      p->dfdmax = 1.0 * (p->num2 - p->num1) / SP_FT_MAXLEN;
    }
    return SP_OK;
}


int sporth_jitter(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out;
    SPFLOAT amp;
    SPFLOAT cpsMin;
    SPFLOAT cpsMax;
    sp_jitter *jitter;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "jitter: Creating\n");
#endif

            sp_jitter_create(&jitter);
            plumber_add_ugen(pd, SPORTH_JITTER, jitter);
            if(sporth_check_args(stack, "fff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for jitter\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            cpsMax = sporth_stack_pop_float(stack);
            cpsMin = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "jitter: Initialising\n");
#endif
            cpsMax = sporth_stack_pop_float(stack);
            cpsMin = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            jitter = pd->last->ud;
            sp_jitter_init(pd->sp, jitter);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            cpsMax = sporth_stack_pop_float(stack);
            cpsMin = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            jitter = pd->last->ud;
            jitter->amp = amp;
            jitter->cpsMin = cpsMin;
            jitter->cpsMax = cpsMax;
            sp_jitter_compute(pd->sp, jitter, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            jitter = pd->last->ud;
            sp_jitter_destroy(&jitter);
            break;
        default:
            plumber_print(pd, "jitter: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
