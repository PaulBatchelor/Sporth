#include "plumber.h"

#define STACKSIZE 32

typedef struct {
    int ninputs;
    int noutputs;
    const char *str;
    int id;
} sporthdot_entry;

typedef struct {
    char label[256];
    int tick;    
} sporthdot_stacklet;

sporthdot_entry tbl[] = {
#define SPORTH_UGEN(key, func, macro, ninputs, noutputs) \
    {ninputs, noutputs, key},
#include "ugens.h"
#undef SPORTH_UGEN
    {-1, -1, NULL}
};

typedef struct {
    sp_data *sp;
    plumber_data pd;
    int pipe_pos;
    int stack_pos;
    sporthdot_stacklet stack[STACKSIZE];
    FILE *fp;
} sporthdot;

void sd_init(sporthdot *sd)
{
    int i;
    sd->stack_pos = 0;

    for(i = 0; i < STACKSIZE; i++) {
        sd->stack[i].tick = 0;
    }

    sd->pipe_pos = 0;
    sd->fp = stdout;
}

void sd_pop(sporthdot *sd, const char *func)
{
    sporthdot_stacklet *stack = &sd->stack[sd->stack_pos - 1];
    printf("%s -> %s_%d\n", stack->label, func, sd->pipe_pos);
    stack->tick--;
    if(stack->tick <= 0) {
        sd->stack_pos--;
    }
}

void sd_push_float(sporthdot *sd, SPFLOAT flt)
{
    sd->stack_pos++;
    sporthdot_stacklet *stack = &sd->stack[sd->stack_pos - 1];
    stack->tick = 1;
    sprintf(stack->label, "flt_%d", sd->pipe_pos);
    fprintf(sd->fp, "%s [label=\"%g\"]\n", stack->label, flt);
}

void sd_push_string(sporthdot *sd, const char *str)
{
    sd->stack_pos++;
    sporthdot_stacklet *stack = &sd->stack[sd->stack_pos - 1];
    stack->tick = 1;
    sprintf(stack->label, "string_%d", sd->pipe_pos);
    fprintf(sd->fp, "%s [label=\"\'%s\'\", shape=\"trapezium\"]\n", stack->label, str);
}

void sd_push_func(sporthdot *sd, uint32_t type)
{
    int i;

    sporthdot_entry *ent = &tbl[type - SPORTH_FOFFSET];

    char *var = (char *)ent->str;
    char *label = (char *)ent->str;


    switch(type) {
        case SPORTH_ADD:
            var = (char *)tbl[SPORTH_ADDv2 - SPORTH_FOFFSET].str;
            break;
        case SPORTH_DIV:
            var = (char *)tbl[SPORTH_DIVv2 - SPORTH_FOFFSET].str;
            break;
        case SPORTH_MUL:
            var = (char *)tbl[SPORTH_MULv2 - SPORTH_FOFFSET].str;
            break;
        case SPORTH_SUB:
            var = (char *)tbl[SPORTH_SUBv2 - SPORTH_FOFFSET].str;
            break;
        default:
            break;
    }

    fprintf(sd->fp, "%s_%d [label=\"%s\",shape=\"box\"]\n", 
            var, sd->pipe_pos, label);

    for(i = 0; i < ent->ninputs; i++) {
        sd_pop(sd, var);
    }
    
    if(ent->noutputs != 0) {
        sd->stack_pos++;
    }

    sporthdot_stacklet *stack = &sd->stack[sd->stack_pos - 1];
    stack->tick = ent->noutputs;
    sprintf(stack->label, "%s_%d", var, sd->pipe_pos);  
}


void sd_go(sporthdot *sd)
{
    plumber_data *plumb = &sd->pd;
    plumbing *pipes = plumb->pipes;
    uint32_t n;
    plumber_pipe *pipe, *next;
    pipe = pipes->root.next;
    SPFLOAT *ptr = NULL;
    fprintf(sd->fp, "digraph G {\n");
    fprintf(sd->fp, "ordering=out\n");
    fprintf(sd->fp, "rankdir=LR\n");
    for(n = 0; n < pipes->npipes; n++) {
        next = pipe->next;
        switch(pipe->type) {
            case SPORTH_FLOAT:
                ptr = (SPFLOAT *)pipe->ud;
                sd_push_float(sd, *ptr);
                break;
            case SPORTH_STRING:
                sd_push_string(sd, (char *)pipe->ud);
                break;
            default:
                sd_push_func(sd, pipe->type);
                break;
        }
        pipe = next;
        sd->pipe_pos++;
    }
    fprintf(sd->fp, "}\n");
}

int main(int argc, char *argv[])
{
    if(argc < 2) {
        fprintf(stderr, "Error\n");
        return 0;
    }
    sporthdot sd;

    sd_init(&sd);
    sp_data *sp = sd.sp;
    sp_create(&sp);
    plumber_data *pd = &sd.pd;
    plumber_register(pd);
    plumber_init(pd);
    pd->sp = sp;
    plumber_open_file(pd, argv[1]);
    plumber_parse(pd);
    plumber_compute(pd, PLUMBER_INIT);

    sd_go(&sd);

    plumber_close_file(pd);
    plumber_clean(pd);
    sp_destroy(&sp);
    return 0;
}
