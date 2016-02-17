#include <soundpipe.h>
#include <sporth.h>

long counter = 0;

void process(sp_data *sp, void *ud)
{
    plumber_data *pd = ud;
    if(++counter == sp->sr * 3) {
        printf("we are here...\n");
        /*TODO: fix memory leaks while not compiled properly */
        /* plumber_recompile_string(pd, "mtof 0.5 saw"); */
        plumber_recompile_string(pd, "0.5 sine");
    }
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
