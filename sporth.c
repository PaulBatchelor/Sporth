#include <stdio.h>
#include <stdlib.h>
#include "plumber.h"

enum {
    DRIVER_FILE,
    DRIVER_RAW
};

void process(sp_data *sp, void *ud){
    plumber_data *pd = ud;
    plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out = 0;
    int chan;
    for (chan = 0; chan < pd->nchan; chan++) {
        out = sporth_stack_pop_float(&pd->sporth.stack);
        sp->out[chan] = out;
    }
}

uint32_t str2time(plumber_data *pd, char *str)
{
    int len = strlen(str);
    char last = str[len - 1];
    switch(last) {
        case 's':
            str[len - 1] = '\0';
            return atof(str) * pd->sp->sr;
            break;
        default:
            return atoi(str);
            break;
    }
}

int main(int argc, char *argv[])
{
    static plumber_data plumb_g;

    char filename[60];
    sprintf(filename, "test.wav");
    unsigned long len = 5 * 44100;
    int sr = 44100;
    int nchan = 1;
    char *time = NULL;
    *argv++;
    argc--;
    int driver = DRIVER_FILE;
    while(argc > 0 && argv[0][0] == '-') {
        switch(argv[0][1]){
            case 'd':
                if(--argc) {
                    *argv++;
#ifdef DEBUG_MODE
                    printf("setting duration to %s\n", argv[0]);
#endif
                    //len = atol(argv[0]);
                    time = argv[0];
                } else {
                    printf("There was a problem setting the length..\n");
                    exit(1);
                }
                break;
            case 'o':
                if(--argc) {
                    *argv++;
#ifdef DEBUG_MODE
                    printf("setting filename to %s\n", argv[0]);
#endif
                    strncpy(filename, argv[0], 60);
                } else {
                    printf("There was a problem setting the length..\n");
                    exit(1);
                }
                break;
            case 'r':
                if(--argc) {
                    *argv++;
#ifdef DEBUG_MODE
                    printf("setting samplerate to %s\n", argv[0]);
#endif
                    sr = atoi(argv[0]);
                } else {
                    printf("There was a problem setting the samplerate..\n");
                    exit(1);
                }
                break;
            case 'c':
                if(--argc) {
                    *argv++;
#ifdef DEBUG_MODE
                    printf("setting nchannels to %s\n", argv[0]);
#endif
                    nchan = atoi(argv[0]);
                } else {
                    printf("There was a problem setting the channels..\n");
                    exit(1);
                }
                break;
            case 'b':
                if(--argc) {
                    *argv++;
                    if (!strcmp(argv[0], "file")) {
                        driver = DRIVER_FILE;
                    } else if ((!strcmp(argv[0], "raw"))) {
                        driver = DRIVER_RAW;
                    } else {
                        printf("Could not find driver \"%s\".\n", argv[0]);
                        exit(1);
                    }
                } else {
                    printf("There was a problem setting the driver..\n");
                    exit(1);
                }
                break;
            case 'h':
                printf("Usage: sporth input.sp\n");
                exit(1);
                break;
            default:
                printf("default.. \n");
                exit(1);
                break;
        }
        *argv++;
        argc--;
    }

    FILE *fp;

    if(argc == 0) {
        fp = stdin;
    } else {
        fp = fopen(argv[0], "r");
        if(fp == NULL) {
            fprintf(stderr,
                    "There was an issue opening the file %s.\n", argv[0]);
            exit(1);
        }
    }

    plumber_register(&plumb_g);
    plumber_init(&plumb_g);
    plumb_g.nchan = nchan;
    srand(plumb_g.seed);
    sp_data *sp;

    sp_createn(&sp, plumb_g.nchan);
    plumb_g.sp = sp;
    sprintf(sp->filename, "%s", filename);
    sp->sr = sr;
    if(time != NULL) sp->len = str2time(&plumb_g, time);

    if(plumber_parse(&plumb_g, fp) == PLUMBER_OK){
        plumber_compute(&plumb_g, PLUMBER_INIT);
        plumb_g.sporth.stack.pos = 0;
#ifdef DEBUG_MODE
        plumber_show_pipes(&plumb_g);
#endif
        switch(driver) {
            case DRIVER_FILE:
                sp_process(sp, &plumb_g, process);
                break;
            case DRIVER_RAW:
                sp_process_raw(sp, &plumb_g, process);
                break;
            default:
                sp_process(sp, &plumb_g, process);
                break;
        }
    }
    if(plumb_g.sporth.stack.error > 0) {
        printf("Uh-oh! Sporth created %d error(s).\n",
                plumb_g.sporth.stack.error);
    }
    plumber_clean(&plumb_g);
    fclose(fp);
    sp_destroy(&sp);
    return 0;
}
