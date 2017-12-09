#include <soundpipe.h>
#include <sporth.h>

long counter = 0;

void process(sp_data *sp, void *ud)
{
    plumber_data *pd = ud;
    counter++;
    if(counter == sp->sr * 3) {
        plumber_print(pd, "Recompiling!\n");
        plumber_recompile_string(pd, "60 mtof 0.5 saw"); 
    } else if(counter == sp->sr * 3.5) {
        plumber_print(pd, 
            "Trying to compile a string with too few arguments\n");
        plumber_recompile_string(pd, "mtof 0.5 saw"); 
    } else if(counter == sp->sr * 3.8) {
        plumber_print(pd, "Trying to compile a patch with stack overflow\n");
        plumber_recompile_string(pd, "0 0"); 
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
    /* turn on stack checking to make it mono */
    plumber_check_stack(&pd, 1);
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
