#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"
#include "macros.h"
#include "modules.h"
plumber_data plumb_g;
#include "flist.h"

void osc_compute(sp_data *sp, void *ud){
    plumber_data *pd = ud;
    plumber_parse(pd, PLUMBER_COMPUTE);
    SPFLOAT out;
    out = sporth_stack_pop_float(&pd->sporth.stack);
    sp->out[0] = out;
}


int main(int argc, char *argv[])
{
    if(argc == 1) {
       printf("Usage: sporth input.sp\n"); 
       return 1;
    }
    sporth_htable_init(&plumb_g.sporth.dict);
    sporth_register_func(&plumb_g.sporth, flist); 
    sp_data *sp;
    sp_create(&sp);
    plumber_init(&plumb_g);
    plumb_g.sp = sp;
    sp->len = 44100;
    if(sporth_parse(&plumb_g.sporth, argv[1]) == SPORTH_OK){
        plumb_g.sporth.stack.pos = 0;
        sp_process(sp, &plumb_g, osc_compute);
    }
    plumber_clean(&plumb_g);
    sp_destroy(&sp);
    return 0;
}
