#include "plumber.h"

int plumber_init(plumber_data *plumb)
{
    plumb->mode = PLUMBER_CREATE;
    plumb->last = &plumb->root;
    plumb->npipes = 0;
    return PLUMBER_OK;
}

int plumber_parse(plumber_data *plumb, int mode)
{
    plumb->mode = mode;
    plumber_pipe *pipe = plumb->root.next, *next;
    uint32_t n;
    float *fval;
    int rc;
    sporth_data *sporth = &plumb->sporth;
    for(n = 0; n < plumb->npipes; n++) {
        next = pipe->next;
        switch(pipe->type) {
            case SPORTH_FLOAT:
                fval = pipe->ud;
                if(sporth_stack_push_float(&sporth->stack, *fval) == SPORTH_NOTOK){
                    //plumb->mode = PLUMBER_PANIC;
                    return PLUMBER_NOTOK; 
                };
                break;
            default:
                plumb->last = pipe;
                rc = sporth->flist[pipe->type - SPORTH_FOFFSET].func(&sporth->stack, 
                        sporth->flist[pipe->type - SPORTH_FOFFSET].ud);
                if (rc == PLUMBER_NOTOK) {
                    //plumb->mode = PLUMBER_PANIC;
                }
                break;
        } 
       pipe = next; 
    }
    return PLUMBER_OK;
}

int plumber_clean(plumber_data *plumb)
{
    uint32_t n;
    plumber_parse(plumb, PLUMBER_DESTROY);
    plumber_pipe *pipe, *next;
    pipe = plumb->root.next;
    for(n = 0; n < plumb->npipes; n++) {
        next = pipe->next;
        if(pipe->type == SPORTH_FLOAT || pipe->type == SPORTH_STRING) 
            free(pipe->ud);
        free(pipe);
        pipe = next;
    } 
    sporth_htable_destroy(&plumb->sporth.dict);
    return PLUMBER_OK;
}

int plumber_add_float(plumber_data *plumb, float num)
{
    plumber_pipe *new = malloc(sizeof(plumber_pipe));
  
    if(new == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    new->type = SPORTH_FLOAT;
    new->size = sizeof(SPFLOAT);
    new->ud = malloc(new->size);
    float *val = new->ud;
    *val = num;
    if(new->ud == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    plumb->last->next = new;
    plumb->last = new;
    plumb->npipes++;
    return PLUMBER_OK;
}

int plumber_add_module(plumber_data *plumb, 
        uint32_t id, size_t size, void *ud)
{
    plumber_pipe *new = malloc(sizeof(plumber_pipe));
  
    if(new == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    new->type = id;
    new->size = size;
    new->ud = ud;

    plumb->last->next = new;
    plumb->last = new;
    plumb->npipes++;
    return PLUMBER_OK;
}
