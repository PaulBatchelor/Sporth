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
    SPFLOAT *freq, *amp;
    sp_data *sp;
    sp_create(&sp);
    plumber_data pd;
    plumber_register(&pd);
    plumber_init(&pd);
    pd.sp = sp;

    plumber_create_var(&pd, "freq", &freq);
    plumber_create_var(&pd, "amp", &amp);

    *freq = 1000;
    *amp = 0.5;

    plumber_parse_string(&pd, "_freq get _amp get sine");
    //plumber_parse_string(&pd, "440 0.5 sine");
    plumber_show_pipes(&pd);
    plumber_compute(&pd, PLUMBER_INIT);

    sp_process(sp, &pd, process);

    plumber_clean(&pd);
    sp_destroy(&sp);
    return 0;
}
