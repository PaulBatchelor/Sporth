
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

int sporth_metatable_create(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int size;
    sp_ftbl *ft;
    char *str;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_METATABLE_CREATE, NULL);

            if(sporth_check_args(stack, "sf") != SPORTH_OK) {
                fprintf(stderr, "metatable_create: not enough arguments\n");
                return PLUMBER_NOTOK;
            }
            size = (int)sporth_stack_pop_float(stack);
            str = sporth_stack_pop_string(stack);

            free(str);
            break;

        case PLUMBER_INIT:
            size = (int)sporth_stack_pop_float(stack);
            str = sporth_stack_pop_string(stack);
            free(str);
            break;

        case PLUMBER_COMPUTE:
            size = (int)sporth_stack_pop_float(stack);
            break;

        case PLUMBER_DESTROY:
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}

int sporth_metatable_add(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int pos;
    sp_ftbl *ft;
    char *metatable, *table;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_METATABLE_ADD, NULL);

            if(sporth_check_args(stack, "ssf") != SPORTH_OK) {
                fprintf(stderr, "metatable_add: not enough arguments\n");
                return PLUMBER_NOTOK;
            }
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);

            free(metatable);
            free(table);
            break;

        case PLUMBER_INIT:
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            free(metatable);
            free(table);
            break;

        case PLUMBER_COMPUTE:
            pos = (int)sporth_stack_pop_float(stack);
            break;

        case PLUMBER_DESTROY:
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}

int sporth_metatable_copy(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int pos;
    sp_ftbl *ft;
    char *metatable, *table;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_METATABLE_COPY, NULL);

            if(sporth_check_args(stack, "ssf") != SPORTH_OK) {
                fprintf(stderr, "metatable_add: not enough arguments\n");
                return PLUMBER_NOTOK;
            }
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);

            free(metatable);
            free(table);
            break;

        case PLUMBER_INIT:
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            free(metatable);
            free(table);
            break;

        case PLUMBER_COMPUTE:
            pos = (int)sporth_stack_pop_float(stack);
            break;

        case PLUMBER_DESTROY:
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}
