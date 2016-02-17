/* 
 * ftable.c
 * This example will compile Sporth code that references a Soundpipe ftable
 *
 * To compile run:
 *
 * gcc ftable.c -lsporth -lsoundpipe -lsndfile -lm -o ftable
 *
 */
#include <soundpipe.h>
#include <sporth.h>

typedef struct {
    sp_data *sp;
    plumber_data pd;
    sp_ftbl *seq;
} UserData;

static void process(sp_data *sp, void *udp)
{
    UserData *ud = udp;
    plumber_data *pd = &ud->pd;
    plumber_compute(pd, PLUMBER_COMPUTE);
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}

int main() 
{
    UserData ud;

    /* allocate and initialize */
    sp_create(&ud.sp);
    plumber_register(&ud.pd);
    plumber_init(&ud.pd);

    /* create an ftable with a size 1, which will be expanded with gen_vals */
    sp_ftbl_create(ud.sp, &ud.seq, 1);
    /* Gen_vals will create a table from a string of numbers */
    sp_gen_vals(ud.sp, ud.seq, "60 63 67 72");
    
    /* Sporth code to be parsed, using a ftable generated in c called 'seq' */
    char *str = 
        "0.1 dmetro "
        "0 'seq' tseq "
        "0.01 port mtof "
        "0.5 1 1 1 fm";

    /* Tell plumber that that we will take care of removing the following ftables */
    /* This is needed for persistant ftables that don't get cleaned for 
     * recompilation */
    plumber_ftmap_delete(&ud.pd, 0);
    /* add ftable to plumber and call it "seq" */
    plumber_ftmap_add(&ud.pd, "seq", ud.seq);
    /* Turn automatic deletion back "on" */
    plumber_ftmap_delete(&ud.pd, 1);

    /* make plumber know about the soundpipe instance */
    ud.pd.sp = ud.sp;

    /* parse string and allocate plumbing */
    plumber_parse_string(&ud.pd, str);

    /* initialize the plumbing */
    plumber_compute(&ud.pd, PLUMBER_INIT);
    /* run */
    sp_process(ud.sp, &ud, process);
    /* Cleanup */
    /* Plumber will handle freeing the memory of the ftable we added */
    /* no need to call sp_ftbl_destroy */
    plumber_clean(&ud.pd);
    sp_ftbl_destroy(&ud.seq);
    sp_destroy(&ud.sp);
    return 0;
}
