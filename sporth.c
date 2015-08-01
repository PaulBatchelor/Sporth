#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"
#include "macros.h"
#include "modules.h"
plumber_data plumb_g;
#include "flist.h"

void osc_compute(sp_data *sp, void *ud){
    plumber_data *pd = ud;
    plumber_compute(pd, PLUMBER_COMPUTE);
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

    unsigned long len = 3 * 44100;
    *argv++; 
    argc--;
    while(argc > 0 && argv[0][0] == '-') {
        switch(argv[0][1]){
            case 'l':
                if(--argc) {
                    printf("setting length\n");
                    len = atol(argv[0]);
                    *argv++;
                } else {
                    printf("There was a problem..\n");
                    exit(1);
                }
                break;
            default: 
                printf("default.. \n");
                break;
        } 
        printf("argpos at %d\n", argc);
        *argv++;
        argc--;
    }

    if(argc <= 0) {
        printf("You must specify a file!\n");
        exit(1);
    }

    sporth_htable_init(&plumb_g.sporth.dict);
    sporth_register_func(&plumb_g.sporth, flist); 
    sp_data *sp;
    sp_create(&sp);
    plumber_init(&plumb_g);
    plumb_g.sp = sp;
    sp->len = len;
    if(plumber_parse(&plumb_g, argv[0]) == SPORTH_OK){
        plumber_compute(&plumb_g, PLUMBER_INIT);
        plumb_g.sporth.stack.pos = 0;
        plumber_show_pipes(&plumb_g);
        sp_process(sp, &plumb_g, osc_compute);
    }
    if(plumb_g.sporth.stack.error > 0) {
        printf("Uh-oh! Sporth created %d error(s).\n", 
                plumb_g.sporth.stack.error);
    }
    plumber_clean(&plumb_g);
    sp_destroy(&sp);
    return 0;
}
