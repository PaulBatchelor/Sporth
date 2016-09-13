#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <jack/jack.h>
#include <time.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFSIZE 2048

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
#ifdef LIVE_CODING
    sporth_listener sl;
#endif
} sp_jack;

void error(char *msg) {
  perror(msg);
  exit(1);
}

static void *start_listening(void *ud)
{
    sporth_listener *sl = ud;
    int portno = sl->portno;
    int sockfd; /* socket */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf1[BUFSIZE]; /* message buf */
    char buf2[BUFSIZE]; /* message buf */
    int whichbuf = 0;
    char *buf; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    uint32_t n; /* message byte size */
    plumber_data *pd = sl->pd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    error("ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, 
    (const void *)&optval , sizeof(int));

    memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);
    if (bind(sockfd, (struct sockaddr *) &serveraddr, 
    sizeof(serveraddr)) < 0) 
    error("ERROR on binding");

    clientlen = sizeof(clientaddr);
    while (sl->start) {

        if(whichbuf == 0) {
            buf = buf1;
        } else {
            buf = buf2;
        }
        memset(buf, 0, BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0,
            (struct sockaddr *) &clientaddr, &clientlen);

        if (n < 0) error("ERROR in recvfrom");

        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr, 

        sizeof(clientaddr.sin_addr.s_addr), AF_INET);

        if (hostp == NULL) error("ERROR on gethostbyaddr");

        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL) error("ERROR on inet_ntoa\n");

        printf("server received datagram from %s (%s)\n", 
        hostp->h_name, hostaddrp);
        printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);
        n = sendto(sockfd, buf, strlen(buf), 0, 
        (struct sockaddr *) &clientaddr, clientlen);
        if (n < 0) error("ERROR in sendto"); 
        pd->str = buf;
        whichbuf = (whichbuf == 0) ? 1 : 0;
        pd->recompile = 1;
    }

    pthread_exit(NULL);
}

static int sp_jack_cb(jack_nframes_t nframes, void *arg)
{
    int i, chan;
    sp_jack *jd = arg;
    if(jd->sl.start == 0) {
        return 0;
    }
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

static void sp_jack_shutdown (void *arg)
{
    exit (1);
}

void sporth_start_listener(sporth_listener *sl)
{
    pthread_create(&sl->thread, NULL, start_listening, sl);
}


int sp_process_jack(plumber_data *pd, 
        void *ud, void (*callback)(sp_data *, void *), int port)
{
    const char **ports;

    const char *server_name = NULL;
    int chan;
    jack_options_t options = JackNullOption;
    jack_status_t status;
    sp_jack jd;
    jd.sp = pd->sp;
    jd.pd = pd;
    pd->ud = &jd;

    char client_name[256];

    sp_data *sp = pd->sp;
    jd.callback = callback;
    jd.ud = ud;

    if(!strcmp(pd->sp->filename, "test.wav")) {
        strncpy(client_name, "soundpipe", 256);
    } else {
        strncpy(client_name, pd->sp->filename, 256);
    }

    pd->sporth.flist[SPORTH_IN - SPORTH_FOFFSET].func = sporth_jack_in;

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
        /* client_name = jack_get_client_name(jd.client[0]); */
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

#ifdef LIVE_CODING
    sporth_listener *sl = &jd.sl;
    sl->portno = port; 
    sl->start = 1;
    sl->pd = pd;
    sporth_start_listener(sl); 
    sl->start = 1;
#endif

    fgetc(stdin);
    free (ports);
    jack_client_close(jd.client[0]);
    free(jd.output_port);
    free(jd.client);


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
