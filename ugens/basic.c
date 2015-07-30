#include "plumber.h"

#include "macros.h"

int sporth_mix(sporth_stack *stack, void *ud) 
{
    plumber_data *pd = ud;
    SPFLOAT val = 0;
    SPFLOAT sum = 0;
    int n;
    switch(pd->mode){
        case PLUMBER_CREATE:
            printf("creating mix function... \n");
            plumber_add_module(pd, SPORTH_MIX, 0, 
                    NULL);
            break;
        case PLUMBER_COMPUTE:
            for(n = 0; n < stack->pos; n++){
                val = sporth_stack_pop_float(stack);
                sum += val;
            }
            sporth_stack_push_float(stack, sum);
            break;
        case PLUMBER_DESTROY:
            break;
        default:
           printf("Error: Unknown mode!"); 
           break;
    }   
    return SPORTH_OK;
}

int sporth_constant(sporth_stack *stack, void *ud) 
{
    plumber_data *pd = ud;

    if(sporth_check_args(stack, "f") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    float val = sporth_stack_pop_float(stack);
    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_float(pd, val);
            break;
        case PLUMBER_COMPUTE:
            break;
        case PLUMBER_DESTROY:
            break;
        default:
           printf("Error: Unknown mode!"); 
           break;
    }   
    sporth_stack_push_float(stack, val);
    return SPORTH_OK;
}

typedef struct {
    sp_osc *osc;
    sp_ftbl *ft;
} sporth_sine_d;

int sporth_sine(sporth_stack *stack, void *ud) 
{
    plumber_data *pd = ud;

    if(sporth_check_args(stack, "ff") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    SPFLOAT amp = sporth_stack_pop_float(stack);
    SPFLOAT freq = sporth_stack_pop_float(stack);
    SPFLOAT out;
    sporth_sine_d *data;
    plumber_pipe *pipe;
    switch(pd->mode){
        case PLUMBER_CREATE:
            printf("creating sine function... \n");
            data = malloc(sizeof(sporth_sine_d));
            sp_osc_create(&data->osc);
            sp_ftbl_create(pd->sp, &data->ft, 4096);
            sp_gen_sine(pd->sp, data->ft);
            sp_osc_init(pd->sp, data->osc, data->ft);
            plumber_add_module(pd, SPORTH_SINE, sizeof(data), 
                    data);
            break;
        case PLUMBER_COMPUTE:
            pipe = pd->last;
            data = pipe->ud;
            data->osc->freq = freq;
            data->osc->amp = amp;
            sp_osc_compute(pd->sp, data->osc, NULL, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            pipe = pd->last;
            data = pipe->ud;
            sp_ftbl_destroy(&data->ft);
            sp_osc_destroy(&data->osc);
            break;
        default:
           printf("Error: Unknown mode!"); 
           break;
    }   
    return SPORTH_OK;
}

int sporth_add(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    float v1 = sporth_stack_pop_float(stack);
    float v2 = sporth_stack_pop_float(stack);
    sporth_stack_push_float(stack, v1 + v2);
    return SPORTH_OK;
}

int sporth_mul(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    float v1 = sporth_stack_pop_float(stack);
    float v2 = sporth_stack_pop_float(stack);
    sporth_stack_push_float(stack, v1 * v2);
    return SPORTH_OK;
}

int sporth_sub(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    float v1 = sporth_stack_pop_float(stack);
    float v2 = sporth_stack_pop_float(stack);
    sporth_stack_push_float(stack, v2 - v1);
    return SPORTH_OK;
}

int sporth_divide(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") != SPORTH_OK) {
        return SPORTH_NOTOK;
    }
    float v1 = sporth_stack_pop_float(stack);
    float v2 = sporth_stack_pop_float(stack);
    sporth_stack_push_float(stack, v2 / v1);
    return SPORTH_OK;
}
