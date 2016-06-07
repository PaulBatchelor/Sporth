/* amp.c

   Free software by Richard W.E. Furse. Do with as you will. No
   warranty.

   This LADSPA plugin provides simple mono and stereo amplifiers.

   This file has poor memory protection. Failures during malloc() will
   not recover nicely. */

/*****************************************************************************/

#include <stdlib.h>
#include <string.h>

#include <soundpipe.h>
#include <sporth.h>
#include <lo/lo.h>

/*****************************************************************************/

#include "ladspa.h"

/*****************************************************************************/

/* The port numbers for the plugin: */

#define NARGS 8
#define NPORTS NARGS + 4

enum{ 
LS_P1,
LS_P2,
LS_P3,
LS_P4,
LS_P5,
LS_P6,
LS_P7,
LS_P8,
AMP_INPUT1,
AMP_OUTPUT1,
AMP_INPUT2,
AMP_OUTPUT2
};

/*****************************************************************************/

/* The structure used to hold port connection information and state
   (actually gain controls require no further state). */

typedef struct {
    plumber_data pd;
    sp_data *sp;
    int recompile, error;
    char *str;
    plumber_argtbl *at;
} UserData;


typedef struct {

  LADSPA_Data * m_pfInputBuffer1;
  LADSPA_Data * m_pfOutputBuffer1;
  LADSPA_Data * m_pfInputBuffer2;
  LADSPA_Data * m_pfOutputBuffer2;

  UserData ud;
} LADsporth;

static lo_server_thread st;
static LADsporth *g_ls = NULL;

static void error(int num, const char *m, const char *path)
{
    printf("liblo server error %d in path %s: %s\n", num, path, m);
    //printf("error\n");
    fflush(stdout);
}

int quit_handler(const char *path, const char *types, lo_arg ** argv,
                 int argc, void *data, void *user_data)
{
    printf("quiting\n\n");
    fflush(stdout);

    return 0;
}

int sporth_handler(const char *path, const char *types, lo_arg ** argv,
                int argc, void *data, void *user_data)
{
    printf("Sporth string: %s\n", &argv[0]->s);

    if(g_ls == NULL) {
        return 0;
    } 

    UserData *ud = &g_ls->ud;

    if(!ud->recompile) {
        plumber_open_file(&ud->pd, &argv[0]->s);
        ud->recompile = 1;
    }

    fflush(stdout);

    return 0;
}


LADSPA_Handle 
instantiateLADsporth(const LADSPA_Descriptor * Descriptor,
		     unsigned long             SampleRate) {
    LADsporth *ls = g_ls;
    UserData *ud = &ls->ud;
    sp_data *sp = ud->sp;

    sp->sr = SampleRate;

    plumber_ftmap_delete(&ud->pd, 0);
    plumber_ftmap_add_userdata(&ud->pd, "ls", (void *)ud->at);
    plumber_ftmap_delete(&ud->pd, 1);

    plumber_open_file(&ud->pd, "file.sp");
    plumber_parse(&ud->pd);
    plumber_close_file(&ud->pd);
    plumber_compute(&ud->pd, PLUMBER_INIT);
    ud->recompile = 0;
    return (LADSPA_Handle)ls;
}

void 
connectPortToLADsporth(LADSPA_Handle Instance,
		       unsigned long Port,
		       LADSPA_Data * DataLocation) {

  LADsporth * psLADsporth;
  psLADsporth = (LADsporth *)Instance;
  UserData *ud = &psLADsporth->ud;
  switch (Port) {
      case LS_P1:
        ud->at->tbl[0] = DataLocation;
        break;
      case LS_P2:
        ud->at->tbl[1] = DataLocation;
        break;
      case LS_P3:
        ud->at->tbl[2] = DataLocation;
        break;
      case LS_P4:
        ud->at->tbl[3] = DataLocation;
        break;
      case LS_P5:
        ud->at->tbl[4] = DataLocation;
        break;
      case LS_P6:
        ud->at->tbl[5] = DataLocation;
        break;
      case LS_P7:
        ud->at->tbl[6] = DataLocation;
        break;
      case LS_P8:
        ud->at->tbl[7] = DataLocation;
        break;
      case AMP_INPUT1:
        psLADsporth->m_pfInputBuffer1 = (SPFLOAT *)DataLocation;
        break;
      case AMP_OUTPUT1:
        psLADsporth->m_pfOutputBuffer1 = DataLocation;
        break;
      case AMP_INPUT2:
        psLADsporth->m_pfInputBuffer2 = DataLocation;
        break;
      case AMP_OUTPUT2:
        psLADsporth->m_pfOutputBuffer2 = DataLocation;
        break;
  }
}

static void ls_compute(plumber_data *pd, SPFLOAT **tbl,
        SPFLOAT *in1, SPFLOAT *in2,
        SPFLOAT *out1, SPFLOAT *out2, 
        int offset) 
{
        tbl[offset] = in1;
        tbl[offset + 1] = in2;
        tbl[offset + 2] = out1;
        tbl[offset + 3] = out2;
        plumber_compute(pd, PLUMBER_COMPUTE);
}

void 
runMonoLADsporth(LADSPA_Handle Instance,
		 unsigned long SampleCount) {
    LADSPA_Data * pfInput, *pfInput2;
    LADSPA_Data * pfOutput, *pfOutput2;
    LADsporth * psLADsporth;
    unsigned long lSampleIndex;

    psLADsporth = (LADsporth *)Instance;
    UserData *ud = &psLADsporth->ud;
    plumber_data *pd = &ud->pd;
    SPFLOAT in1 = 0, in2 = 0, out1 = 0, out2 = 0;

    if(ud->recompile) {
        printf("compiling string\n");
        int error;
        plumber_reinit(&ud->pd);
        plumber_ftmap_delete(&ud->pd, 0);
        plumber_ftmap_add_userdata(&ud->pd, "ls", (void *)ud->at);
        plumber_ftmap_delete(&ud->pd, 1);
        error = plumber_reparse(&ud->pd);
        plumber_swap(&ud->pd, error);
        plumber_close_file(&ud->pd);
        ud->recompile = 0;
    }

    pfInput = psLADsporth->m_pfInputBuffer1;
    pfOutput = psLADsporth->m_pfOutputBuffer1;
    pfInput2 = psLADsporth->m_pfInputBuffer2;
    pfOutput2 = psLADsporth->m_pfOutputBuffer2;
    for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++) {
        ls_compute(pd,ud->at->tbl, 
                pfInput++, pfInput2++, 
                pfOutput++, pfOutput2++, 16);
    }
}


void 
cleanupLADsporth(LADSPA_Handle Instance) {
}

LADSPA_Descriptor * g_psMonoDescriptor = NULL;

static void register_port(LADSPA_PortDescriptor *pdesc, 
        LADSPA_PortRangeHint *phint,
        char **pnames, int id) 
{
    char name[5];
    sprintf(name, "P%d", id);
    pdesc[id]
      = LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL;
    pnames[id]
      = strdup(name);
    phint[id].HintDescriptor
      = (LADSPA_HINT_BOUNDED_BELOW 
	 | LADSPA_HINT_BOUNDED_ABOVE
	 | LADSPA_HINT_DEFAULT_0);
    phint[id].LowerBound 
      = 0;
    phint[id].UpperBound
      = 1;
}

static void register_audio_ports(LADSPA_PortDescriptor *piPortDescriptors,
        LADSPA_PortRangeHint *psPortRangeHints,
        char **pcPortNames,
        int in1,
        int in2,
        int out1,
        int out2)
{
    piPortDescriptors[in1]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[out1]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[in2]
      = LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO;
    piPortDescriptors[out2]
      = LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO;
    pcPortNames[in1]
      = strdup("Input");
    pcPortNames[out1]
      = strdup("Output");
    pcPortNames[in2]
      = strdup("Input2");
    pcPortNames[out2]
      = strdup("Output2");
    psPortRangeHints[in1].HintDescriptor
      = 0;
    psPortRangeHints[out1].HintDescriptor
      = 0;
    psPortRangeHints[in2].HintDescriptor
      = 0;
    psPortRangeHints[out2].HintDescriptor
      = 0;
}

void _init() 
{


    char ** pcPortNames;
    LADSPA_PortDescriptor * piPortDescriptors;
    LADSPA_PortRangeHint * psPortRangeHints;

    g_psMonoDescriptor
    = (LADSPA_Descriptor *)malloc(sizeof(LADSPA_Descriptor));
    printf("LADSporth: Initializing!\n");
    st = lo_server_thread_new("6449", error);
    lo_server_thread_add_method(st, "/sporth/", "s", sporth_handler, NULL);
    lo_server_thread_start(st);

    if (g_psMonoDescriptor) {
        g_psMonoDescriptor->UniqueID
          = 2222;
        g_psMonoDescriptor->Label
          = strdup("LADsporth");
        g_psMonoDescriptor->Properties
          = LADSPA_PROPERTY_HARD_RT_CAPABLE;
        g_psMonoDescriptor->Name 
          = strdup("Sporth Plugin");
        g_psMonoDescriptor->Maker
          = strdup("Paul Batchelor");
        g_psMonoDescriptor->Copyright
          = strdup("None");
        g_psMonoDescriptor->PortCount
          = NPORTS;

        piPortDescriptors
          = (LADSPA_PortDescriptor *)calloc(NPORTS, sizeof(LADSPA_PortDescriptor));
        g_psMonoDescriptor->PortDescriptors
          = (const LADSPA_PortDescriptor *)piPortDescriptors;

        pcPortNames
          = (char **)calloc(NPORTS, sizeof(char *));
        g_psMonoDescriptor->PortNames 
          = (const char **)pcPortNames;

        psPortRangeHints = ((LADSPA_PortRangeHint *)
                calloc(NPORTS, sizeof(LADSPA_PortRangeHint)));
        g_psMonoDescriptor->PortRangeHints
          = (const LADSPA_PortRangeHint *)psPortRangeHints;

        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P1);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P2);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P3);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P4);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P5);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P6);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P7);
        register_port(piPortDescriptors, psPortRangeHints, pcPortNames, LS_P8);
        register_audio_ports(piPortDescriptors, 
                psPortRangeHints, 
                pcPortNames,
                AMP_INPUT1,
                AMP_INPUT2,
                AMP_OUTPUT1,
                AMP_OUTPUT2);
        g_psMonoDescriptor->instantiate 
          = instantiateLADsporth;
        g_psMonoDescriptor->connect_port 
          = connectPortToLADsporth;
        g_psMonoDescriptor->activate
          = NULL;
        g_psMonoDescriptor->run
          = runMonoLADsporth;
        g_psMonoDescriptor->run_adding
          = NULL;
        g_psMonoDescriptor->set_run_adding_gain
          = NULL;
        g_psMonoDescriptor->deactivate
          = NULL;
        g_psMonoDescriptor->cleanup
          = cleanupLADsporth;
    }

    LADSPA_Handle *h = malloc(sizeof(LADsporth));
    LADsporth *ls = (LADsporth *)h;
    g_ls = ls;

    UserData *ud = &ls->ud;
    sp_data *sp;
    sp_create(&sp);
    ud->sp = sp;
    ud->pd.sp = sp;
    plumber_init(&ud->pd);
    plumber_register(&ud->pd);

    plumber_argtbl_create(&ud->pd, &ud->at, 20);
  
}

void
deleteDescriptor(LADSPA_Descriptor * psDescriptor) {
  unsigned long lIndex;
  if (psDescriptor) {
    free((char *)psDescriptor->Label);
    free((char *)psDescriptor->Name);
    free((char *)psDescriptor->Maker);
    free((char *)psDescriptor->Copyright);
    free((LADSPA_PortDescriptor *)psDescriptor->PortDescriptors);
    for (lIndex = 0; lIndex < psDescriptor->PortCount; lIndex++)
      free((char *)(psDescriptor->PortNames[lIndex]));
    free((char **)psDescriptor->PortNames);
    free((LADSPA_PortRangeHint *)psDescriptor->PortRangeHints);
    free(psDescriptor);
  }
}
void _fini() {
    UserData *ud;
    LADsporth *ls= g_ls;
    ud = &ls->ud;
    if(ud->sp != NULL) {
        plumber_argtbl_destroy(&ud->pd, &ud->at);
        plumber_clean(&ud->pd);
        sp_destroy(&ud->sp);
    }
    deleteDescriptor(g_psMonoDescriptor);
    lo_server_thread_free(st);
}

const LADSPA_Descriptor * 
ladspa_descriptor(unsigned long Index) {
  /* Return the requested descriptor or null if the index is out of
     range. */
  switch (Index) {
  case 0:
    return g_psMonoDescriptor;
  default:
    return NULL;
  }
}

