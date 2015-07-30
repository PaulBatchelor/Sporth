#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"

plumber_data &plumb_g;

enum {
SPORTH_ADD = SPORTH_FOFFSET,
SPORTH_SUB,
SPORTH_DIV,
SPORTH_MUL,
SPORTH_SINE,
SPORTH_CONSTANT,
SPORTH_MIX
};

static sporth_func flist[] = {
{"add", add, NULL},
{"sub", sub, NULL},
{"mul", mul, NULL},
{"div", divide, NULL},
{"sine", sporth_sine, &plumb_g},
{"c", sporth_constant, &plumb_g},
{"mix", sporth_mix, &plumb_g},
{NULL, NULL, NULL}
};

void osc_compute(sp_data *sp, void *ud){
    plumber_data *pd = ud;
    plumber_parse(pd, PLUMBER_COMPUTE);
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}

int main()
{
    sporth_htable_init(&plumb_g.sporth.dict);
    sporth_register_func(&plumb_g.sporth, flist); 
    sp_data *sp;
    sp_create(&sp);
    plumber_init(&plumb_g);
    plumb_g.sp = sp;
    sporth_parse(&plumb_g.sporth, "sine.txt");
    sp_process(sp, &plumb_g, osc_compute);
    printf("the last pushed value on the stack is %g\n",
            sporth_stack_pop_float(&plumb_g.sporth.stack));
    plumber_clean(&plumb_g);
    sp_destroy(&sp);
    return 0;
}
