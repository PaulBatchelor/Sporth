#include <stdlib.h>
#include <math.h>

#include "plumber.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    SPFLOAT freq;
    SPFLOAT res;

    SPFLOAT delay[6];
    SPFLOAT tanhstg[3];
    SPFLOAT oldfreq;
    SPFLOAT oldres;
    SPFLOAT oldacr;
    SPFLOAT oldtune;
    int sr;
} sp_moogladder;

static double tanh_approx(double x)
{
    int sign = 1;

    if (x < 0) {
        sign=-1;
        x= -x;
    }

    if (x >= 4.0) return sign;
    if (x < 0.5) return x*sign;

    return sign*tanh(x);
}

static int moogladder_create(sp_moogladder **t){
    *t = malloc(sizeof(sp_moogladder));
    return SP_OK;
}

static int moogladder_destroy(sp_moogladder **t){
    free(*t);
    return SP_OK;
}

static int moogladder_init(sp_moogladder *p, int sr){
    int i;

    p->res = 0.4;
    p->freq = 1000;
    p->sr = sr;

    for (i = 0; i < 6; i++) p->delay[i] = 0.0;
    for (i = 0; i < 3; i++) p->tanhstg[i] = 0.0;
    p->oldfreq = 0.0;
    p->oldres = -1.0;
    return SP_OK;
}

static SPFLOAT moogladder_compute(sp_moogladder *p,
                                  SPFLOAT in){
    SPFLOAT freq = p->freq;
    SPFLOAT res = p->res;
    SPFLOAT res4;
    SPFLOAT *delay = p->delay;
    SPFLOAT *tanhstg = p->tanhstg;
    SPFLOAT stg[4], input;
    SPFLOAT acr, tune;
    SPFLOAT out;

#define THERMAL (0.000025) /* transistor thermal voltage */

    int j, k;

    out = 0;

    if (res < 0) res = 0;

    if (p->oldfreq != freq || p->oldres != res) {
        SPFLOAT f, fc, fc2, fc3, fcr;
        p->oldfreq = freq;
        fc =  (SPFLOAT)(freq/p->sr);
        f  =  0.5*fc;
        fc2 = fc*fc;
        fc3 = fc2*fc;
        fcr = 1.8730*fc3 + 0.4955*fc2 - 0.6490*fc + 0.9988;
        acr = -3.9364*fc2 + 1.8409*fc + 0.9968;
        tune = (1.0 - exp(-((2 * M_PI)*f*fcr))) / THERMAL;
        p->oldres = res;
        p->oldacr = acr;
        p->oldtune = tune;
    } else {
        res = p->oldres;
        acr = p->oldacr;
        tune = p->oldtune;
    }

    res4 = 4.0*(SPFLOAT)res*acr;

    for (j = 0; j < 2; j++) {
        input = in - res4*delay[5];
        delay[0] = stg[0] = delay[0] + tune*(tanh_approx(input*THERMAL) - tanhstg[0]);
        for (k = 1; k < 4; k++) {
          input = stg[k-1];
          stg[k] = delay[k]
            + tune*((tanhstg[k-1] = tanh_approx(input*THERMAL))
                    - (k != 3 ? tanhstg[k] : tanh_approx(delay[k]*THERMAL)));
          delay[k] = stg[k];
        }
        delay[5] = (stg[3] + delay[4])*0.5;
        delay[4] = stg[3];
    }
    out = (SPFLOAT) delay[5];
    return out;
}

int sporth_moogladder(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT input;
    SPFLOAT out;
    SPFLOAT freq;
    SPFLOAT res;
    sp_moogladder *moogladder;

    switch (pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "moogladder: Creating\n");
#endif

            moogladder_create(&moogladder);
            plumber_add_ugen(pd, SPORTH_MOOGLADDER, moogladder);
            if (sporth_check_args(stack, "fff") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for moogladder\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            res = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "moogladder: Initialising\n");
#endif
            res = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            moogladder = pd->last->ud;
            moogladder_init(moogladder, pd->sp->sr);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            res = sporth_stack_pop_float(stack);
            freq = sporth_stack_pop_float(stack);
            input = sporth_stack_pop_float(stack);
            moogladder = pd->last->ud;
            moogladder->freq = freq;
            moogladder->res = res;
            out = moogladder_compute(moogladder, input);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            moogladder = pd->last->ud;
            moogladder_destroy(&moogladder);
            break;
        default:
            plumber_print(pd, "moogladder: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
