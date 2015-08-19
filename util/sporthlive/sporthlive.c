#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "plumber.h"

typedef struct {
    plumber_data pd;
    int counter;
    time_t ltime;
} UserData;

static void process(sp_data *sp, void *udata){
    UserData *ud = udata;
    plumber_data *pd = &ud->pd;
    int rc;
    if(ud->counter == 0) {
        struct stat attrib;
        stat(pd->filename, &attrib);
        if(attrib.st_mtime != ud->ltime) {
            plumber_recompile(pd);
            ud->ltime = attrib.st_mtime;
        }
    }

    rc = plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out = 0;
    int chan;
    for (chan = 0; chan < pd->nchan; chan++) {
        if(rc == PLUMBER_OK) {
            out = sporth_stack_pop_float(&pd->sporth.stack);
        }
        sp->out[chan] = out;
    }
    ud->counter++;
    ud->counter %= 4096;
}

int main(int argc, char *argv[])
{
    UserData ud;
    ud.counter = 0;
    plumber_init(&ud.pd);
    struct stat attrib;
    ud.ltime = 0;
    sporth_run(&ud.pd, argc, argv, &ud, process);
    return 0;
}
