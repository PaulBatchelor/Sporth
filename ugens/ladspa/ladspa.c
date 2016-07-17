#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "plumber.h"


//#ifdef BUILD_LADSPA
#include "ladspa.h"
#include "utils.h"

#define LADSPA_PORT_GROUP LADSPA_PORT_AUDIO << 1 /* 16 */

typedef struct {
    LADSPA_Descriptor_Function pfDescriptorFunction;
    LADSPA_Handle *handle;
    const LADSPA_Descriptor * psDescriptor;
    void * pvPluginHandle;
    sp_ftbl *argtbl;
    uint32_t arg_count;
    uint32_t ninputs;
    uint32_t noutputs;
    SPFLOAT *in;
    SPFLOAT *out;
} sporth_ladspa_d;

static void pop_inputs(plumber_data *pd, sporth_stack *stack, sporth_ladspa_d *l)
{
    uint32_t i;
    for(i = 0; i < l->ninputs; i++) {
        l->in[i] = sporth_stack_pop_float(stack);
    }
}

static void push_outputs(plumber_data *pd, sporth_stack *stack, sporth_ladspa_d *l)
{
    uint32_t i;
    for(i = 0; i < l->noutputs ; i++) {
        sporth_stack_push_float(stack, l->out[i]);
    }
}

static int init_ladspa(plumber_data *pd, sporth_stack *stack, sporth_ladspa_d *l, char *plugin, char *ftname, uint32_t id)
{
    uint32_t i = 0;
    uint32_t ac = 0, ic = 0, oc = 0;
    l->pvPluginHandle = loadLADSPAPluginLibrary(plugin);
    dlerror();
    l->pfDescriptorFunction 
    = (LADSPA_Descriptor_Function)dlsym(l->pvPluginHandle, "ladspa_descriptor");
    if (!l->pfDescriptorFunction) {
    const char * pcError = dlerror();
    if (pcError) 
      fprintf(stderr,
          "Unable to find ladspa_descriptor() function in plugin file "
          "\"%s\": %s.\n"
          "Are you sure this is a LADSPA plugin file?\n", 
          plugin,
          pcError);
        return PLUMBER_NOTOK;
    }

    l->psDescriptor = l->pfDescriptorFunction(id);
    l->handle = l->psDescriptor->instantiate(l->psDescriptor, pd->sp->sr);
    if(l->psDescriptor->activate) {
        l->psDescriptor->activate(l->handle);
    }

    fprintf(stderr, "Loading LADSPA plugin \"%s\"\n", l->psDescriptor->Label);

    for(i = 0; i < l->psDescriptor->PortCount; i++) {

        switch(l->psDescriptor->PortDescriptors[i]) {
            case (LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL | LADSPA_PORT_GROUP):
            case (LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL):
                fprintf(stderr, "Control %d: %s\n", ac, l->psDescriptor->PortNames[i]);
                ac++;
                break;
            case (LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO):
                ic++;
                break;
            case (LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO):
                oc++;
                break;
            default:
                break;
        }
    }

    l->arg_count = ac;
    l->ninputs = ic;
    l->noutputs = oc;

    l->in = malloc(sizeof(SPFLOAT) * ic);
    l->out = malloc(sizeof(SPFLOAT) * oc);

    if(plumber_ftmap_search(pd, ftname, &l->argtbl) == PLUMBER_NOTOK) {
        stack->error++;
        return PLUMBER_NOTOK;
    }

    if(l->argtbl->size < l->arg_count) {
        fprintf(stderr, 
                "LADSPA: table \"%s\" should have a size of at least %d",
                ftname, l->arg_count);
        return PLUMBER_NOTOK;
    }

    ac = 0;
    ic = 0;
    oc = 0;

    /* Do it again, for the bindings */
     
    for(i = 0; i < l->psDescriptor->PortCount; i++) {

        switch(l->psDescriptor->PortDescriptors[i]) {
            case (LADSPA_PORT_INPUT | LADSPA_PORT_CONTROL):
                l->psDescriptor->connect_port(l->handle, i, (LADSPA_Data *)&l->argtbl->tbl[ac]);
                l->argtbl->tbl[ac] = 0;
                ac++;
                break;
            case (LADSPA_PORT_INPUT | LADSPA_PORT_AUDIO):
                l->psDescriptor->connect_port(l->handle, i, (LADSPA_Data *)&l->in[ic]);
                l->in[ic] = 0;
                ic++;
                break;
            case (LADSPA_PORT_OUTPUT | LADSPA_PORT_AUDIO):
                l->psDescriptor->connect_port(l->handle, i, (LADSPA_Data *)&l->out[oc]);
                l->out[oc] = 0;
                oc++;
                break;
            default:
                break;
        }
    }

    return PLUMBER_OK;
}

static void clean_ladspa(sporth_ladspa_d *l)
{
    l->psDescriptor->cleanup(l->handle);
    unloadLADSPAPluginLibrary(l->pvPluginHandle);
    free(l->out);
    free(l->in);
}
//#endif

int sporth_ladspa(sporth_stack *stack, void *ud)
{
//#ifdef BUILD_LADSPA
    plumber_data *pd = ud;
    sporth_ladspa_d *ladspa;
    char *argtbl;
    char *plugin;
    uint32_t id;

    switch(pd->mode){
        case PLUMBER_CREATE:
            ladspa = malloc(sizeof(sporth_ladspa_d));
            plumber_add_ugen(pd, SPORTH_LADSPA, ladspa);
            if(sporth_check_args(stack, "sfs") != SPORTH_OK) {
                stack->error++;
                fprintf(stderr,"Invalid arguments for LADSPA.\n");
                return PLUMBER_NOTOK;
            }
            argtbl = sporth_stack_pop_string(stack);
            id = (uint32_t) sporth_stack_pop_float(stack);
            plugin = sporth_stack_pop_string(stack);
            if(init_ladspa(pd, stack, ladspa, plugin, argtbl, id) == PLUMBER_NOTOK) {
                return PLUMBER_NOTOK;
            } 
            pop_inputs(pd, stack, ladspa);
            push_outputs(pd, stack, ladspa);
            break;
        case PLUMBER_INIT:
            ladspa = pd->last->ud;
            argtbl = sporth_stack_pop_string(stack);
            id = (uint32_t) sporth_stack_pop_float(stack);
            plugin = sporth_stack_pop_string(stack);
            pop_inputs(pd, stack, ladspa);
            push_outputs(pd, stack, ladspa);
            break;

        case PLUMBER_COMPUTE:
            ladspa = pd->last->ud;
            id = (uint32_t) sporth_stack_pop_float(stack);
            pop_inputs(pd, stack, ladspa);
            ladspa->psDescriptor->run(ladspa->handle, 1);
            push_outputs(pd, stack, ladspa);
            break;
        case PLUMBER_DESTROY:
            ladspa = pd->last->ud;
            clean_ladspa(ladspa);
            free(ladspa);
            break;
        default:
            fprintf(stderr,"Error: Unknown mode!");
            return PLUMBER_NOTOK;
            break;
    }
    return PLUMBER_OK;
//#else
    //return PLUMBER_NOTOK;
//#endif

}
