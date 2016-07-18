#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include "plumber.h"
#include "cdb.h"

typedef struct {
    char *val;
    int *fd;
    cdbi_t vlen;
} sporth_cdb_t;


int sporth_cdb(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    
    sporth_cdb_t *cdb;
    char *key = NULL;
    char *ftname = NULL;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "cdb: Creating\n");
#endif

            cdb = malloc(sizeof(sporth_cdb_t));
            plumber_add_ugen(pd, SPORTH_CDB, cdb);

            if(sporth_check_args(stack, "ss") != SPORTH_OK) {
                fprintf(stderr, "cdb: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }
            ftname = sporth_stack_pop_string(stack);
            key = sporth_stack_pop_string(stack);

            if(plumber_ftmap_search_userdata(
                pd, ftname, (void **)&cdb->fd) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            if(cdb_seek(*cdb->fd, key, strlen(key), &cdb->vlen) > 0) {
                cdb->val = malloc(cdb->vlen + 1);
                cdb_bread(*cdb->fd, cdb->val, cdb->vlen);
            }
            cdb->val[cdb->vlen] = '\0';
            sporth_stack_push_string(stack, &cdb->val);
            break;

        case PLUMBER_INIT:
            
#ifdef DEBUG_MODE
            fprintf(stderr, "cdb: Initialising\n");
#endif
            cdb = pd->last->ud;
            sporth_stack_push_string(stack, &cdb->val);
            break;

        case PLUMBER_COMPUTE:
            break;
        case PLUMBER_DESTROY:
            cdb = pd->last->ud;
            free(cdb->val);
            free(cdb);
            break;
        default:
            fprintf(stderr, "cdb: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int sporth_cdbload(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int *fd;    
    char *ftname;
    char *filename;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "cdb: Creating\n");
#endif

            fd = malloc(sizeof(int));
            plumber_add_ugen(pd, SPORTH_CDBLOAD, fd);

            if(sporth_check_args(stack, "ss") != SPORTH_OK) {
                fprintf(stderr, "cdbload: not enough/wrong arguments\n");
                return PLUMBER_NOTOK;
            }
            filename = sporth_stack_pop_string(stack);
            ftname = sporth_stack_pop_string(stack);
            
             
            *fd = open(filename, O_RDONLY);
            plumber_ftmap_add_userdata(pd, ftname, fd);
            break;

        case PLUMBER_INIT:
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            
#ifdef DEBUG_MODE
            fprintf(stderr, "cdbload: Initialising\n");
#endif
            break;

        case PLUMBER_COMPUTE:
            break;
        case PLUMBER_DESTROY:
            fd = pd->last->ud;
            close(*fd);
            break;
        default:
            fprintf(stderr, "cdbload: Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
