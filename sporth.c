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
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}


int main(int argc, char *argv[])
{
    if(argc == 1) {
       printf("Usage: sporth input.txt\n"); 
       return 1;
    }
    sporth_htable_init(&plumb_g.sporth.dict);
    sporth_register_func(&plumb_g.sporth, flist); 
    sp_data *sp;
    sp_create(&sp);
    plumber_init(&plumb_g);
    plumb_g.sp = sp;
    if(sporth_parse(&plumb_g.sporth, argv[1]) == SPORTH_OK){
        sp_process(sp, &plumb_g, osc_compute);
    }
    plumber_clean(&plumb_g);
    sp_destroy(&sp);
    return 0;
}
