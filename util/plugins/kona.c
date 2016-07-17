/* Kona
 *
 * This plugin allows you evaluate kona strings and write the result to
 * an ftable in sporth, to be used by something like tseq. 
 *
 * To compile (on Linux):
 *
 * gcc kona.c -lsporth -lsoundpipe -lkona -lpthread -I $KONA_PATH/src/ -L $KONA_PATH -fPIC -shared -o kona.so 
 *
 * Where $KONA_PATH is the directory where the kona source code is.
 * 
 * Some boilerplate sporth code could look like this:
 * 
 *   "./kona.so" "kona" fl
 *   "maj" "0 2 4 7" "kona" fe
 *   3 metro  0 "maj" tseq 60 + mtof 0.5 sine
 *   "kona" fc
 * 
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <kona.h>
#include <soundpipe.h>
#include <sporth.h>


static int sporth_gen_kona(plumber_data *pd, sporth_stack *stack, void **ud)
{
    int n; 
    sp_ftbl *ft;
    char *str;
    char *ftname;

    switch(pd->mode){
        case PLUMBER_CREATE:
            plumber_add_ugen(pd, SPORTH_KONA, NULL);

            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                fprintf(stderr, "Kona: not enough arguments.\n");
                return PLUMBER_NOTOK;
            }
            str = sporth_stack_pop_string(stack);
            ftname = sporth_stack_pop_string(stack);
            fprintf(stderr, "Evaluating Kona string '%s'\n", str);
            srand(time(NULL));
            ksk("", 0);
            K ints = ksk(str, 0);
            I *tbl = KI(ints);
            sp_ftbl_create(pd->sp, &ft, ints->n);
            for(n = 0; n < ft->size; n++) {
                ft->tbl[n] = (SPFLOAT)tbl[n];
            }
            plumber_ftmap_add(pd, ftname, ft);
            break;

        case PLUMBER_INIT:

            ftname = sporth_stack_pop_string(stack);
            str = sporth_stack_pop_string(stack);
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

plumber_dyn_func sporth_return_ugen()
{
    return sporth_gen_kona;
}
