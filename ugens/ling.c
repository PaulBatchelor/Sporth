#include <stdlib.h>
#include "string.h"
#include "plumber.h"
#include "ling.h"

typedef struct {
    ling_data ling;
    char str[500];
    uint32_t val;
} sp_ling;

int sporth_ling(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    SPFLOAT tick = 0; 
    sp_ling *ling; 
    ling_func *fl = NULL; 
    char *str;
    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "ling: Creating\n");
#endif
            ling = malloc(sizeof(sp_ling));
            plumber_add_module(pd, SPORTH_LING, sizeof(sp_ling), ling);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "ling: Initialising\n");
#endif

            if(sporth_check_args(stack, "fs") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for ling\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            ling = pd->last->ud;

            str = sporth_stack_pop_string(stack);
            tick = sporth_stack_pop_float(stack);
            ling_init(&ling->ling);
            fl = ling_create_flist(&ling->ling);
            ling_register_func(&ling->ling, fl);
            ling->val = 0;
            strncpy(ling->str, str, 500);
            sporth_stack_push_float(stack, 0);
#ifdef DEBUG_MODE
            printf("ling: the string is %s\n", ling->str);
#endif
            ling_parse_line(&ling->ling, ling->str);
            free(str);
            break;
        case PLUMBER_COMPUTE:
            ling = pd->last->ud;
            tick = sporth_stack_pop_float(stack);
            if(tick != 0) {
                //ling_parse_line(&ling->ling, ling->str);
                ling_seq_run(&ling->ling);
                ling->val = ling_stack_pop(&ling->ling.stack);
                ling->ling.t++;
            } 
            sporth_stack_push_float(stack, (SPFLOAT)ling->val);
            break;
        case PLUMBER_DESTROY:
            ling = pd->last->ud;
            ling_destroy(&ling->ling);
            free(ling);
            break;
        default:
            fprintf(stderr, "ling: Uknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
