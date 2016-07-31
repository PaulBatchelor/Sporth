#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <time.h>
#include <string.h>
#include <lo/lo.h>

#include "plumber.h"

static int sporth_jack_in(sporth_stack *stack, void *ud);

typedef struct {
    sp_data *sp;
    plumber_data *pd;
    jack_port_t **output_port;
    jack_port_t *input_port;
    jack_client_t **client;
    SPFLOAT in;
    void *ud;
    void (*callback)(sp_data *, void *);
} sp_jack;

static int sp_jack_cb(jack_nframes_t nframes, void *arg)
{
    int i, chan;
    sp_jack *jd = arg;
    jack_default_audio_sample_t  *out[jd->sp->nchan];
    jack_default_audio_sample_t  *in;

    in = jack_port_get_buffer(jd->input_port, nframes);

    for(chan = 0; chan < jd->sp->nchan; chan++)
        out[chan] = jack_port_get_buffer (jd->output_port[chan], nframes);

    for(i = 0; i < nframes; i++){
        jd->in = in[i];
        jd->callback(jd->sp, jd->ud);
        for(chan = 0; chan < jd->sp->nchan; chan++)
        out[chan][i] = jd->sp->out[chan];
    }
    return 0;
}

static void error(int num, const char *m, const char *path)
{
    fflush(stdout);
}

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data)
{
    printf("quiting\n\n");
    fflush(stdout);
    return 0;
}

static int sporth_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    fprintf(stderr, "Sporth string: %s\n", &argv[0]->s);
    
    sp_jack *ud = user_data;
    plumber_data *pd = ud->pd;

    if(!pd->recompile) {
        pd->str = strdup(&argv[0]->s);
        pd->recompile = 1;
    }

    fflush(stdout);

    return 0;
}

static int sporth_pset(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    sp_jack *ud = user_data;
    plumber_data *pd = ud->pd;


    int pval = argv[0]->i % 16;
    SPFLOAT val = argv[1]->f;

    pd->p[pval] = val;

    return 0;
}

static void sp_jack_shutdown (void *arg)
{
    exit (1);
}

int sp_process_jack(plumber_data *pd, void *ud, void (*callback)(sp_data *, void *))
{
    const char **ports;
    const char *client_name = "soundpipe";
    const char *server_name = NULL;
    int chan;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    sp_jack jd;
    jd.sp = pd->sp;
    jd.pd = pd;
    pd->ud = &jd;
    sp_data *sp = pd->sp;
    jd.callback = callback;
    jd.ud = ud;

    pd->sporth.flist[SPORTH_IN - SPORTH_FOFFSET].func = sporth_jack_in;

    jd.output_port = malloc(sizeof(jack_port_t *) * sp->nchan);
    jd.client = malloc(sizeof(jack_client_t *));
    
    lo_server_thread st = lo_server_thread_new("6449", error);
    lo_server_thread_add_method(st, "/sporth/eval", "s", sporth_handler, &jd);
    lo_server_thread_add_method(st, "/sporth/pset", "if", sporth_pset, &jd);
    lo_server_thread_start(st);

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

    jd.input_port = jack_port_register(jd.client[0], "sp_input",
            JACK_DEFAULT_AUDIO_TYPE,
            JackPortIsInput, 0);

    for(chan = 0; chan < sp->nchan; chan++) {
        sprintf(chan_name, "output_%d", chan);
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

    lo_server_thread_free(st);

    return SP_OK;
}

static int sporth_jack_in(sporth_stack *stack, void *ud)
{
    plumber_data *pd = (plumber_data *) ud;

    sp_jack * data = (sp_jack *) pd->ud;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "JACK IN: creating\n");
#endif
            plumber_add_ugen(pd, SPORTH_IN, NULL);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "JACK IN: initialising.\n");
#endif

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            sporth_stack_push_float(stack, data->in);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "JACK IN: destroying.\n");
#endif

            break;

        default:
            fprintf(stderr, "Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
