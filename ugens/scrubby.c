#include <stdlib.h>
#include <math.h>
#include "plumber.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    SPFLOAT time, amp, pitch, iN,
        idecim, onset, offset, dbthresh;
    int cnt, hsize, curframe, N, decim,tscale;
    SPFLOAT pos;
    SPFLOAT accum;
    SPFLOAT *fwin, *bwin, *prev;
    SPFLOAT *outframe, *win, *nwin;
    int *framecount;
    SPFLOAT *tab;
    int curbuf;
    SPFLOAT resamp;
    sp_ftbl *ft;
    sp_fft fft;
} sp_scrubby;

int sp_scrubby_create(sp_scrubby **p);
int sp_scrubby_destroy(sp_scrubby **p);
int sp_scrubby_init(sp_data *sp,
                    sp_scrubby *p,
                    sp_ftbl *ft,
                    int winsize);
int sp_scrubby_compute(sp_data *sp,
                       sp_scrubby *p,
                       SPFLOAT *out);


int sp_scrubby_create(sp_scrubby **p)
{
    *p = malloc(sizeof(sp_scrubby));
    return SP_OK;
}

int sp_scrubby_destroy(sp_scrubby **p)
{
    sp_scrubby *pp = *p;
    sp_fft_destroy(&pp->fft);
    free((*p)->fwin);
    free((*p)->bwin);
    free((*p)->prev);
    free((*p)->framecount);
    free((*p)->outframe);
    free((*p)->win);
    free(*p);
    return SP_OK;
}

static int find_power(int n) {
    int pow = -1;
    while (n > 0) {
        n >>= 1;
        pow++;
    }
    return pow;
}

int sp_scrubby_init(sp_data *sp, sp_scrubby *p, sp_ftbl *ft, int winsize)
{
    int N, ui;
    unsigned int size;
    int decim;
    int pow;

    p->ft = ft;
    p->idecim = 4;
    p->iN = winsize;
    p->pitch = 1;
    p->amp = 1;
    p->time = 0;

    decim = p->idecim;

    N =  p->iN;

    /* find power to use for fft, log2(winsize) */

    pow = find_power(winsize);

    sp_fft_init(&p->fft, pow);


    if (decim == 0) decim = 4;

    p->hsize = N/decim;
    p->cnt = p->hsize;
    p->curframe = 0;
    p->pos = 0;

    size = (N+2)*sizeof(SPFLOAT);

    p->fwin = calloc(1, size);
    p->bwin = calloc(1, size);
    p->prev = calloc(1, size);

    size = decim*sizeof(int);

    p->framecount = calloc(1, size);

    {
      int k=0;
        for (k=0; k < decim; k++) {
            p->framecount[k] = k*N;
        }
    }

    size = decim*sizeof(SPFLOAT)*N;
    p->outframe = calloc(1, size);

    size = N*sizeof(SPFLOAT);
    p->win = calloc(1, size);

    {
        SPFLOAT x = 2.0 * M_PI/N;
        for (ui=0; ui < N; ui++)
        p->win[ui] = 0.5 - 0.5 * cos((SPFLOAT)ui*x);
    }

    p->N = N;
    p->decim = decim;

    return SP_OK;
}

int sp_scrubby_compute(sp_data *sp, sp_scrubby *p, SPFLOAT *out)
{
    SPFLOAT pitch, time, amp;
    SPFLOAT *tab, frac;
    sp_ftbl *ft;
    int N, hsize, cnt;
    int sizefrs, size, post, i;
    long spos;
    SPFLOAT pos;
    SPFLOAT *fwin, *bwin, insig = 0,
    *prev, *win;
    SPFLOAT *outframe;
    SPFLOAT ph_real, ph_im, tmp_real, tmp_im, divi;
    int *framecnt;
    int curframe, decim;
    SPFLOAT scaling;


    pitch = p->pitch;
    time = p->time;
    amp =p->amp;
    ft = p->ft;
    N = p->N;
    hsize = p->hsize;
    cnt = p->cnt;

    spos = p->pos;
    insig = 0,
    win = p->win;
    curframe = p->curframe;
    decim = p->decim;

    scaling = (8./decim)/3.;

    if (cnt == hsize) {
        tab = ft->tbl;
        size = (int)ft->size;

        spos  = hsize*(long)((time)*sp->sr/hsize);
        sizefrs = size;

        while (spos > sizefrs) spos -= sizefrs;
        while (spos <= 0)  spos += sizefrs;

        pos = spos;
        bwin = p->bwin;
        fwin = p->fwin;
        prev = p->prev;
        framecnt  = p->framecount;
        outframe= p->outframe;

        for (i = 0; i < N; i++) {
            post = (int) pos;
            frac = pos  - post;
            while (post < 0) post += size;
            while (post >= size) post -= size;
            if(post + 1 <  size)
            insig = tab[post] + frac*(tab[post+ 1] - tab[post]);
            else insig = tab[post];

            fwin[i] = insig * win[i];
            post = (int) (pos - hsize*pitch);
            post *= 1;
            post += 0;
            while (post < 0) post += size;
            while (post >= size) post -= size;
            if(post + 1<  size)
            insig = tab[post] + frac*(tab[post + 1] - tab[post]);
            else insig = tab[post];
            bwin[i] = insig * win[i];
            pos += pitch;
        }

        sp_fftr(&p->fft, bwin, N);
        bwin[N] = bwin[1];
        bwin[N+1] = 0.0;
        sp_fftr(&p->fft, fwin, N);
        fwin[N] = fwin[1];
        fwin[N+1] = 0.0;

        for (i=0; i < N + 2; i+=2) {
            divi = 1.0/(hypot(prev[i], prev[i+1]) + 1e-20);
            ph_real = prev[i]*divi;
            ph_im = prev[i+1]*divi;

            tmp_real = bwin[i] * ph_real + bwin[i+1] * ph_im;
            tmp_im = bwin[i] * ph_im - bwin[i+1] * ph_real;
            bwin[i] = tmp_real;
            bwin[i+1] = tmp_im;
        }

        for (i = 0; i < N + 2; i+=2) {
            if (i > 0) {
                if (i < N) {
                    tmp_real = bwin[i] + bwin[i-2] + bwin[i+2];
                    tmp_im = bwin[i+1] + bwin[i-1] + bwin[i+3];
                } else {
                    tmp_real = bwin[i] + bwin[i-2];
                    tmp_im = 0.0;
                }
            } else {
                tmp_real = bwin[i] + bwin[i+2];
                tmp_im = 0.0;
            }

            tmp_real += 1e-15;
            divi = 1.0/(hypot(tmp_real, tmp_im));

            ph_real = tmp_real*divi;
            ph_im = tmp_im*divi;

            tmp_real = fwin[i] * ph_real - fwin[i+1] * ph_im;
            tmp_im = fwin[i] * ph_im + fwin[i+1] * ph_real;

            prev[i] = fwin[i] = tmp_real;
            prev[i+1] = fwin[i+1] = tmp_im;
        }

        fwin[1] = fwin[N];
        sp_ifftr(&p->fft, fwin, N);
        framecnt[curframe] = curframe*N;

        for (i=0;i<N;i++) {
            outframe[framecnt[curframe]+i] = win[i]*fwin[i];
        }

        cnt=0;
        curframe++;
        if (curframe == decim) curframe = 0;
    }

    framecnt  = p->framecount;
    outframe  = p->outframe;
    *out = (SPFLOAT)0;

    for (i = 0; i < decim; i++) {
        *out += outframe[framecnt[i]];
        framecnt[i]++;
    }

    *out *= amp*scaling;
    cnt++;

    p->cnt = cnt;
    p->curframe = curframe;

    return SP_OK;
}



int sporth_scrubby(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT out = 0;
    sp_ftbl * ft = NULL;
    const char *ftname = NULL;
    SPFLOAT time = 0;
    SPFLOAT amp = 0;
    SPFLOAT pitch = 0;
    SPFLOAT winsize = 0;
    sp_scrubby *scrubby = NULL;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            plumber_print(pd, "scrubby: Creating\n");
#endif

            sp_scrubby_create(&scrubby);
            plumber_add_ugen(pd, SPORTH_SCRUBBY, scrubby);

            if(sporth_check_args(stack, "ffffs") != SPORTH_OK) {
                plumber_print(pd,"Not enough arguments for scrubby\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            ftname = sporth_stack_pop_string(stack);
            winsize = sporth_stack_pop_float(stack);
            pitch = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            time = sporth_stack_pop_float(stack);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            plumber_print(pd, "scrubby: Initialising\n");
#endif

            ftname = sporth_stack_pop_string(stack);
            winsize = sporth_stack_pop_float(stack);
            pitch = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            time = sporth_stack_pop_float(stack);
            scrubby = pd->last->ud;

            if(plumber_ftmap_search(pd, ftname, &ft) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            sp_scrubby_init(pd->sp, scrubby, ft, winsize);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            winsize = sporth_stack_pop_float(stack);
            pitch = sporth_stack_pop_float(stack);
            amp = sporth_stack_pop_float(stack);
            time = sporth_stack_pop_float(stack);
            scrubby = pd->last->ud;
            scrubby->time = time;
            scrubby->amp = amp;
            scrubby->pitch = pitch;
            sp_scrubby_compute(pd->sp, scrubby, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            scrubby = pd->last->ud;
            sp_scrubby_destroy(&scrubby);
            break;
        default:
            plumber_print(pd, "scrubby: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
