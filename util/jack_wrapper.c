#include <stdlib.h>
#include <soundpipe.h>

typedef struct {
    sp_in *in;
    int nchan;
} UserData;

void process(sp_data *sp, void *udata) 
{
    UserData *ud = udata;
    SPFLOAT out = 0;
    if(ud->nchan == 1) {
        sp_in_compute(sp, ud->in, NULL, &out);
        sp->out[0] = out;
        sp->out[1] = out;
    } else if (ud->nchan == 2) {
        sp_in_compute(sp, ud->in, NULL, &out);
        sp->out[0] = out;
        sp_in_compute(sp, ud->in, NULL, &out);
        sp->out[1] = out;
    } else { 
        sp_in_compute(sp, ud->in, NULL, &out);
        sp->out[0] = out;
        sp->out[1] = out;
    }
}

int main(int argc, char *argv[]) 
{
    UserData ud;
    sp_data *sp;

    if(argc == 1) {
        printf("Usage: ./%s nchan\n", argv[0]);
        fflush(stdin);
        exit(1);
    }
    int nchan = atoi(argv[1]);
    sp_createn(&sp, 2);
    ud.nchan = nchan;
    sp_in_create(&ud.in);

    sp_in_init(sp, ud.in);

    sp_jack_process(sp, &ud, process);
    
    sp_in_destroy(&ud.in);
    sp_destroy(&sp);
    return 0;
}
