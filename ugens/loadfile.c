#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

int sporth_loadfile(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    sp_ftbl *ft;
    char *str;
    char *filename;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_module(pd, SPORTH_LOADFILE, 0, NULL);
            break;

        case PLUMBER_INIT:
            if(sporth_check_args(stack, "ss") != SPORTH_OK) {
                fprintf(stderr, "Init: not enough arguments for gen_line\n");
                return PLUMBER_NOTOK;
            }

            filename = sporth_stack_pop_string(stack);
            str = sporth_stack_pop_string(stack);
#ifdef DEBUG_MODE
            fprintf(stderr, "Creating line table %s of size %d\n", str, size);
#endif
            if(sp_ftbl_loadfile(pd->sp, &ft, filename) == SP_NOT_OK) {
                fprintf(stderr, "There was an issue creating the ftable \"%s\".\n", str);
                stack->error++;
                return PLUMBER_NOTOK;
            }
            plumber_ftmap_add(pd, str, ft);
            free(str);
            free(filename);
            break;

        case PLUMBER_COMPUTE:
            break;

        case PLUMBER_DESTROY:
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}
