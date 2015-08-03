#include "plumber.h"
#include "macros.h"

int sporth_tseq(sporth_stack *stack, void *ud) 
{
    if(stack->error > 0) return PLUMBER_NOTOK;

    plumber_data *pd = ud;
    SPFLOAT out = 0, trig = 0;
    char *ftname;
    sp_ftbl *ft;
    sp_tseq *tseq;
    switch(pd->mode){
        case PLUMBER_CREATE:
#ifdef DEBUG_MODE
            printf("Creating tseq function... \n");
#endif
            sp_tseq_create(&tseq);
            plumber_add_module(pd, SPORTH_TSEQ, sizeof(sp_tseq), tseq);
            break;
        case PLUMBER_INIT:
            if(sporth_check_args(stack, "fs") != SPORTH_OK) {
                stack->error++;
                printf("Invalid arguments for tseq.\n");
                return PLUMBER_NOTOK;
            }
            tseq = pd->last->ud;

            ftname = sporth_stack_pop_string(stack);
            trig = sporth_stack_pop_float(stack);

            if(plumber_ftmap_search(pd, ftname, &ft) == PLUMBER_NOTOK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            sp_tseq_init(pd->sp, tseq, ft);
            sporth_stack_push_float(stack, 0.0);
            free(ftname);
            break;

        case PLUMBER_COMPUTE:
            tseq = pd->last->ud;
            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                stack->error++;
                return PLUMBER_NOTOK;
            }

            trig = sporth_stack_pop_float(stack);

            sp_tseq_compute(pd->sp, tseq, &trig, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            tseq = pd->last->ud;
            sp_tseq_destroy(&tseq);
            break;
        default:
            printf("Error: Unknown mode!"); 
            stack->error++;
            return PLUMBER_NOTOK;
            break;
    }   
    return PLUMBER_OK;
}
