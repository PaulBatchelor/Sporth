/* Using p-registers from C                                             */
/*                                                                      */
/* To compile:                                                          */
/* gcc p_reg.c -lsporth -lsoundpipe -lsndfile -lm -o p_reg              */


#include <soundpipe.h>
#include <sporth.h>

void process(sp_data *sp, void *ud)
{
    plumber_data *pd = ud;
    plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out;
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}

int main()
{
    sp_data *sp;
    sp_create(&sp);
    plumber_data pd;
    plumber_register(&pd);
    plumber_init(&pd);
    pd.sp = sp;
    plumber_parse_string(&pd, "0 p 1 p sine");

/* p-registers are nothing more than an array in the plumber_data data struct */

/* p-register 0 is set to the sine freq (440 Hz) */
    pd.p[0] = 440;

/* p-register 1 is set to the sine amplitude 0 */
    pd.p[1] = 0.5;

    plumber_compute(&pd, PLUMBER_INIT);

    sp_process(sp, &pd, process);

    plumber_clean(&pd);
    sp_destroy(&sp);
    return 0;
}
