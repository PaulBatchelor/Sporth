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
    char *str =
    "'tri' 4096 '0 1 4096 -1' gen_line "
    "0 p 0.2 sine "
    "1 p 0.2 sine "
    "add";
    plumber_parse_string(&pd, str);
    plumber_compute(&pd, PLUMBER_INIT);

    /* Set the p values to constants */

    pd.p[0] = 440;
    pd.p[1] = 350;

    sp_process(sp, &pd, process);

    plumber_clean(&pd);
    sp_destroy(&sp);
    return 0;
}
