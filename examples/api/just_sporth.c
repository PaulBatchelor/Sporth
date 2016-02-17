/* just_sporth.c
 *
 * By Paul Batchelor
 * August 2015
 *
 * This is a basic example of how to utilize the Sporth language mechanics
 * without calling SoundPlumber, Sporth's audio engine. If you are interested
 * in using Sporth as a embedded scripting language, this is a good place to start.
 *
 * When you are left with Sporth, you can build your own dictionary of functions,
 * utilize the parser, and call the basic stack operations: push, pop, and execute.
 *
 * ----
 *
 * Here's how to build a minimal Sporth environment, sans Soundpipe:
 *
 * In the Sporth source directory:
 *
 * mkdir minimal
 * cp h/sporth.h parse.c func. stack.c hash.c minimal
 * gcc -DMINIMAL -c *.c
 * cp ../examples/just_sporth.c .
 * gcc -DMINIMAL just_sporth.c *.o -o just_sporth
 *
 * now run the example:
 *
 * ./just_sporth
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* If you are using /usr/local/include/sporth.h, you will need to include the
 * soundpipe header files for plumber.
 *
 * If you have created a minimal working version of sporth, you will not
 * need to include this. See header README for how to do this.
 */

#ifndef MINIMAL
#include <soundpipe.h>
#endif

#include "sporth.h"

int add(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") == SPORTH_NOTOK) {
        fprintf(stderr, "not enough args for add\n");
        stack->error++;
    }
    float v1, v2;

    v1 = sporth_stack_pop_float(stack);
    v2 = sporth_stack_pop_float(stack);

    sporth_stack_push_float(stack, v1 + v2);

    return SPORTH_OK;
}

int sub(sporth_stack *stack, void *ud)
{
    if(sporth_check_args(stack, "ff") == SPORTH_NOTOK) {
        fprintf(stderr, "not enough args for add\n");
        stack->error++;
    }
    float v1, v2;

    v1 = sporth_stack_pop_float(stack);
    v2 = sporth_stack_pop_float(stack);

    sporth_stack_push_float(stack, v2 - v1);

    return SPORTH_OK;
}

int main() {
    char *out, *tmp;
    uint32_t prev, pos, offset, len;
    uint32_t size;
/* (1.2 + 4) - 2 */
    char *str = "1.2 4 add 2 sub";

    sporth_data sporth;

/* Create the list of functions */
/* Order is key, function, user data */
    sporth_func flist[] = {
        {"add", add, NULL},
        {"sub", sub, NULL},
        {NULL, NULL, NULL}
    };
/* Initialize data struct */
    sporth_init(&sporth);
/* Put keywords and functions into hash table */
    sporth_register_func(&sporth, flist);

/* From this point on, it's tokenizing, lexing, and cleanup.
 *
 * The tokenizer creates tokens using spaces as a delimeter,
 * unless it is a string, where it uses quotes as a
 * delimeter.
 *
 * The Lexer determines if the value is a float, string, function,
 * or if it supposed to be ignored (for comments and things).
 * Otherwise, it throws an error.
 *
 * Once the input string has been properly tokenized and lexed, the
 * rest of Sporth are just stack operations: values get pushed
 * onto the stack, and functions pop values from the stack and
 * push new values on the stack.
 *
 * Some final notes:
 *
 * Sporth *does* support string data types, but not well.
 * your best bet is just to stick to pushing floats.
 *
 * Parsing happens one string at a time. To read from a
 * file, you'd have to write something reads and parses
 * one line at a time. See 'man getline' for an example of this.
 *
 * There is no need to use my parser. It shouldn't be difficult
 * to write a parser using something like Flex/Bison. In fact,
 * if it pleases you, you can just do away with all parsing and
 * just call the basic sporth stack operations.
 *
 */

    pos = 0;
    offset = 0;
    len = 0;
    size = strlen(str);

    printf("Parsing \"%s\"\n", str);

    while(pos < size) {

        out = sporth_tokenizer(&sporth, str, size, &pos);
        len = strlen(out);
        switch(sporth_lexer(&sporth, out, len)) {
            case SPORTH_FLOAT:
                sporth_stack_push_float(&sporth.stack, atof(out));
                break;
            case SPORTH_STRING:
                tmp = out;
                tmp[len - 1] = '\0';
                tmp++;
                sporth_stack_push_string(&sporth.stack, tmp);
                break;
            case SPORTH_FUNC:
                sporth_exec(&sporth, out);
                break;
            case SPORTH_IGNORE:
                break;
            default:
                fprintf(stderr, "Unknown token %s\n", out);
                break;
        }
        free(out);
    }

    printf("The last value pushed to the stack is %g.\n", sporth_stack_pop_float(&sporth.stack));

    sporth_destroy(&sporth);
    return 0;
}
