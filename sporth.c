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
    int chan;
    for (chan = 0; chan < pd->nchan; chan++) {
        out = sporth_stack_pop_float(&pd->sporth.stack);
        sp->out[chan] = out;
    }
}


int main(int argc, char *argv[])
{
    if(argc == 1) {
       printf("Usage: sporth input.sp\n"); 
       return 1;
    }
    char filename[60];
    sprintf(filename, "test.wav");
    unsigned long len = 5 * 44100;
    int sr = 44100;
    int nchan = 1;
    *argv++; 
    argc--;
    while(argc > 0 && argv[0][0] == '-') {
        switch(argv[0][1]){
            case 'l':
                if(--argc) {
                    *argv++;
                    printf("setting length to %s\n", argv[0]);
                    len = atol(argv[0]);
                } else {
                    printf("There was a problem setting the length..\n");
                    exit(1);
                }
                break;
            case 'o':
                if(--argc) {
                    *argv++;
                    printf("setting filename to %s\n", argv[0]);
                    strncpy(filename, argv[0], 60);
                } else {
                    printf("There was a problem setting the length..\n");
                    exit(1);
                }
                break;
            case 'r':
                if(--argc) {
                    *argv++;
                    printf("setting samplerate to %s\n", argv[0]);
                    sr = atoi(argv[0]);
                } else {
                    printf("There was a problem setting the samplerate..\n");
                    exit(1);
                }
                break;
            case 'c':
                if(--argc) {
                    *argv++;
                    printf("setting nchannels to %s\n", argv[0]);
                    nchan = atoi(argv[0]);
                } else {
                    printf("There was a problem setting the samplerate..\n");
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

    plumber_init(&plumb_g);
    plumb_g.nchan = nchan;
    sp_data *sp;

    sp_createn(&sp, plumb_g.nchan);
    //sp_create(&sp);
    plumb_g.sp = sp;
    sprintf(sp->filename, "%s", filename);
    sp->len = len;
    sp->sr = sr;

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
