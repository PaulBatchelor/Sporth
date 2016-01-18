#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "plumber.h"
#include "kona.h"

int sporth_kona(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;

    int n; 
    sp_ftbl *ft;
    char *str;
    char *ftname;

    switch(pd->mode){
        case PLUMBER_CREATE:
#ifdef DEBUG_MODE
            fprintf(stderr, "Kona: create mode\n");
#endif
            plumber_add_ugen(pd, SPORTH_KONA, NULL);
            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                fprintf(stderr, "Kona: not enough arguments.\n");
                return PLUMBER_NOTOK;
            }
            str = sporth_stack_pop_string(stack);
            ftname = sporth_stack_pop_string(stack);
#ifdef DEBUG_MODE
            fprintf(stderr, "Evaluating Kona string '%s'\n", str);
#endif
            srand(time(NULL));
            ksk("", 0);
            K ints = ksk(str, 0);
            I *tbl = KI(ints);
            sp_ftbl_create(pd->sp, &ft, ints->n);
            for(n = 0; n < ft->size; n++) {
                ft->tbl[n] = (SPFLOAT)tbl[n];
            }
            plumber_ftmap_add(pd, ftname, ft);
            free(ftname);
            free(str);
            break;

        case PLUMBER_INIT:

            ftname = sporth_stack_pop_string(stack);
            str = sporth_stack_pop_string(stack);
            free(str);
            free(ftname);
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
