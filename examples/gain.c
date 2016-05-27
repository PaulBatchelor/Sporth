/* gain.c
 * An example of a sporth ugen that can be dynamically loaded using fl, fe, and
 * fc.
 *
 * To compile (on linux):
 * gcc -fPIC -shared gain.c -o gain.so -lsporth
 *
 */

#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>


/* user data. */
typedef struct {
    int bar;
} foo_data;

static int sporth_gain(plumber_data *pd, sporth_stack *stack, void **ud)
{
    foo_data *foo;
    SPFLOAT gain, in;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            fprintf(stderr, "Creating our custom gain plugin!\n");
            if(sporth_check_args(stack, "ff") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            /* malloc and assign address to user data */
            foo = malloc(sizeof(foo_data));
            *ud = foo;
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0.0);
            break;
        case PLUMBER_INIT:
            in = sporth_stack_pop_float(stack);
            gain = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, in * gain);
            break;

        case PLUMBER_COMPUTE:
            in = sporth_stack_pop_float(stack);
            gain = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, in * gain);
            break;

        case PLUMBER_DESTROY:
            foo = *ud;
            free(foo);
            break;
        default:
            fprintf(stderr, "gain: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return sporth_gain;
}
