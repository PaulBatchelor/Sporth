#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <time.h>
#include <string.h>

#include "../h/plumber.h"

#ifndef SRATE 
#define SRATE 16000
#endif 

typedef struct {
    plumber_data pd;
    sp_data *sp;
    int recompile, error;
    char *str;
} UserData;

typedef struct {
    sp_data *sp;
    jack_port_t **output_port;
    jack_client_t **client;
    void *ud;
    void (*callback)(sp_data *, void *);
} sp_jack;

static void process(sp_data *sp, void *udata){
    UserData *ud = udata;
    plumber_data *pd = &ud->pd;

    if(ud->recompile) {
        plumber_recompile_string(&ud->pd, ud->str);
        ud->recompile = 0;
        free(ud->str);
    }

    plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out = 0;
    int chan;
    for (chan = 0; chan < pd->nchan; chan++) {
        out = sporth_stack_pop_float(&pd->sporth.stack);
        sp->out[chan] = out;
    }
}

static int sp_jack_cb(jack_nframes_t nframes, void *arg)
{
    int i, chan;
    sp_jack *jd = arg;
    jack_default_audio_sample_t  *out[jd->sp->nchan];
    for(chan = 0; chan < jd->sp->nchan; chan++)
    out[chan] = jack_port_get_buffer (jd->output_port[chan], nframes);
    for(i = 0; i < nframes; i++){
        jd->callback(jd->sp, jd->ud);
        for(chan = 0; chan < jd->sp->nchan; chan++)
        out[chan][i] = jd->sp->out[chan];
    }
    return 0;
}

void sp_jack_shutdown (void *arg)
{
    exit (1);
}


int sp_jack_process(sp_data *sp, void *ud, void (*callback)(sp_data *, void *))
{
    const char **ports;
    const char *client_name = "soundpipe";
    const char *server_name = NULL;
    int chan;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    sp_jack jd;
    jd.sp = sp;
    jd.callback = callback;
    jd.ud = ud;

    jd.output_port = malloc(sizeof(jack_port_t *) * sp->nchan);
    jd.client = malloc(sizeof(jack_client_t *));

    jd.client[0] = jack_client_open (client_name, options, &status, server_name);
    if (jd.client[0] == NULL) {
        fprintf (stderr, "jack_client_open() failed, "
             "status = 0x%2.0x\n", status);
        if (status & JackServerFailed) {
            fprintf (stderr, "Unable to connect to JACK server\n");
        }
        exit (1);
    }
    if (status & JackServerStarted) {
        fprintf (stderr, "JACK server started\n");
    }
    if (status & JackNameNotUnique) {
        client_name = jack_get_client_name(jd.client[0]);
        fprintf (stderr, "unique name `%s' assigned\n", client_name);
    }
    jack_set_process_callback (jd.client[0], sp_jack_cb, &jd);
    jack_on_shutdown (jd.client[0], sp_jack_shutdown, 0);

    char chan_name[50];
    for(chan = 0; chan < sp->nchan; chan++) {
        sprintf(chan_name, "output_%d", chan);
        printf("registering %s\n", chan_name);
        jd.output_port[chan] = jack_port_register (jd.client[0], chan_name,
                          JACK_DEFAULT_AUDIO_TYPE,
                          JackPortIsOutput, chan);

        if (jd.output_port[chan] == NULL) {
            fprintf(stderr, "no more JACK ports available\n");
            exit (1);
        }

        if (jack_activate (jd.client[0])) {
            fprintf (stderr, "cannot activate client");
            exit (1);
        }
    }
    ports = jack_get_ports (jd.client[0], NULL, NULL,
                JackPortIsPhysical|JackPortIsInput);
    if (ports == NULL) {
        fprintf(stderr, "no physical playback ports\n");
        exit (1);
    }
    for(chan = 0; chan < sp->nchan; chan++) {
        if (jack_connect (jd.client[0], jack_port_name (jd.output_port[chan]), ports[chan])) {
            fprintf (stderr, "cannot connect output ports\n");
        }
    }
    fgetc(stdin);
    free (ports);
    jack_client_close(jd.client[0]);
    free(jd.output_port);
    free(jd.client);

    return SP_OK;
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Usage: %s file.sp nchan\n", argv[0]);
        return 1;
    }

    int nchan = 1;

    FILE *fp;

    if(argc > 2) {
        nchan = atoi(argv[1]);
        fp = fopen(argv[2], "r");
    } else {
        fp = fopen(argv[1], "r");
    }


    if(fp == NULL) {
        fprintf(stderr, "Couldn't open file %s.\n", argv[1]);
        return 1;
    }

    UserData ud;
    ud.recompile = 0;
    plumber_init(&ud.pd);
    plumber_register(&ud.pd);

    if(nchan == 2) {
        sp_createn(&ud.sp, 2);
        ud.pd.nchan = 2;
    } else {
        sp_create(&ud.sp);
    }
    
    
    sp_srand(ud.sp, time(NULL));
    ud.sp->sr = SRATE;
    ud.pd.sp = ud.sp;
    ud.pd.fp = fp;

    if(plumber_parse(&ud.pd) == PLUMBER_OK) {
        plumber_compute(&ud.pd, PLUMBER_INIT);
        sp_jack_process(ud.sp, &ud, process);
    } else {
        fprintf(stderr, "Errors. Bye\n");
    }


    sp_destroy(&ud.sp);
    plumber_clean(&ud.pd);

    return 0;
}
