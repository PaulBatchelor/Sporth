
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

typedef struct {
    sp_ftbl **list;
    sp_ftbl **target;
    int size;
    int pos;
    int ppos;
} sporth_metatable_d;

int sporth_metatable_create(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int size;
    sporth_metatable_d *md;
    sp_ftbl **list;
    char *str;

    switch(pd->mode){
        case PLUMBER_CREATE:
            md = malloc(sizeof(sporth_metatable_d));
            plumber_add_ugen(pd, SPORTH_METATABLE_CREATE, md);
            if(sporth_check_args(stack, "sf") != SPORTH_OK) {
                fprintf(stderr, "metatable_create: not enough arguments\n");
                return PLUMBER_NOTOK;
            }
            size = (int)sporth_stack_pop_float(stack);
            str = sporth_stack_pop_string(stack);
            md->size = size;
            list = malloc(sizeof(sp_ftbl *) * size);
            md->list = list;

            plumber_ftmap_add_userdata(pd, str, md);
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
            md = pd->last->ud;
            free(md->list);
            //free(md);
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
    sporth_metatable_d *md;
    sporth_metatable_d *md_ref;
    sp_ftbl *ft;
    char *metatable, *table;

    switch(pd->mode){
        case PLUMBER_CREATE:
            md = malloc(sizeof(sporth_metatable_d));
            plumber_add_ugen(pd, SPORTH_METATABLE_ADD, md);

            if(sporth_check_args(stack, "ssf") != SPORTH_OK) {
                fprintf(stderr, "metatable_add: not enough arguments\n");
                return PLUMBER_NOTOK;
            }
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);

            if(plumber_ftmap_search_userdata(pd, metatable, (void **)&md_ref) == PLUMBER_NOTOK) {
                free(metatable);
                free(table);
                stack->error++;
                return PLUMBER_NOTOK;
            }

            if(plumber_ftmap_search(pd, table, &ft) == PLUMBER_NOTOK) {
                free(metatable);
                free(table);
                stack->error++;
                return PLUMBER_NOTOK;
            }

            md->list = md_ref->list;
            md->size = md_ref->size;
            if(pos < md->size) {
                md->list[pos] = ft;
            } else {
                fprintf(stderr, 
                    "Metatable: Position %d exceeds boundary of size %d\n",
                    pos, md->size);
            }
            free(metatable);
            free(table);
            break;

        case PLUMBER_INIT:
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);
            free(metatable);
            free(table);
            break;

        case PLUMBER_COMPUTE:
            pos = (int)sporth_stack_pop_float(stack);
            break;

        case PLUMBER_DESTROY:
            md = pd->last->ud;
            free(md);
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
    sporth_metatable_d *md;
    sporth_metatable_d *md_ref;
    char *metatable, *table;

    switch(pd->mode){
        case PLUMBER_CREATE:
            md = malloc(sizeof(sporth_metatable_d));
            plumber_add_ugen(pd, SPORTH_METATABLE_COPY, md);

            if(sporth_check_args(stack, "ssf") != SPORTH_OK) {
                fprintf(stderr, "metatable_add: not enough arguments\n");
                return PLUMBER_NOTOK;
            }

            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);

            if(plumber_ftmap_search_userdata(pd, metatable, (void **)&md_ref) == PLUMBER_NOTOK) {
                free(metatable);
                free(table);
                stack->error++;
                return PLUMBER_NOTOK;
            }
            
            if(pos < md_ref->size) {
                plumber_ftmap_delete(pd, 0);
                plumber_ftmap_add(pd, table, md_ref->list[pos]);
                plumber_ftmap_delete(pd, 1);
            } else {
                fprintf(stderr, "metatable_copy: position exceeds boundary!\n");
            }

            free(metatable);
            free(table);
            break;

        case PLUMBER_INIT:
            pos = (int)sporth_stack_pop_float(stack);
            metatable = sporth_stack_pop_string(stack);
            table = sporth_stack_pop_string(stack);
            free(metatable);
            free(table);
            break;

        case PLUMBER_COMPUTE:
            pos = (int)sporth_stack_pop_float(stack);
            break;

        case PLUMBER_DESTROY:
            md = pd->last->ud;
            free(md);
            break;

        default:
           printf("Error: Unknown mode!");
           break;
    }
    return PLUMBER_OK;
}
