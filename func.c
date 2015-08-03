#include "sporth.h"

#define LENGTH(x) ((int)(sizeof(x) / sizeof *(x)))

int sporth_register_func(sporth_data *sporth, sporth_func *flist)
{
    sporth->flist = flist;
    uint32_t i = 0;
    while(sporth->flist[i].name != NULL) {
#ifdef DEBUG_MODE
        printf("Registering function \"%s\" at position %d\n", sporth->flist[i].name, i);
#endif
        sporth_htable_add(&sporth->dict, sporth->flist[i].name, i);
        i++;
    }
    sporth->nfunc = i;
}

int sporth_exec(sporth_data *sporth, const char *keyword)
{
    uint32_t id;
    if(sporth_search(&sporth->dict, keyword, &id) != SPORTH_OK) {
        printf("Could not find function called '%s'.\n", keyword);
        return SPORTH_NOTOK;
    }
#ifdef DEBUG_MODE
    printf("Executing function \"%s\"\n", keyword);
#endif
    sporth->flist[id].func(&sporth->stack, sporth->flist[id].ud);
    return SPORTH_OK;
}


int sporth_check_args(sporth_stack *stack, const char *args)
{
    if(stack->error > 0) return SPORTH_NOTOK;

    int len = strlen(args);
    int i;
    if(len > stack->pos) {
        printf("Expected %d arguments on the stack, but there are only %d!\n",
                len, stack->pos);
        stack->error++;
        return SPORTH_NOTOK;
    }
    int pos = stack->pos - len;
    for(i = 0; i < len; i++) {
        switch(args[i]) {
            case 'f':
                if(stack->stack[pos].type != SPORTH_FLOAT) {
                    printf("Argument %d was expecting a float\n", i);
                    stack->error++;
                    return SPORTH_NOTOK;
                }
                break;
            case 's':
                if(stack->stack[pos].type != SPORTH_STRING) {
                    printf("Argument %d was expecting a string, got value %g instead\n", 
                            i, stack->stack[pos].fval);
                    stack->error++;
                    return SPORTH_NOTOK;
                }
                break;
        }
        pos++;
    }

    return SPORTH_OK;
}

//int main()
//{
//    sporth_data sporth;
//
//    sporth_htable_init(&sporth.dict);
//
//    static sporth_func flist[] = {
//        {"add", add, &GlobalData},
//        {"sub", sub, NULL},
//        {"mul", mul, NULL},
//        {"div", divide, NULL},
//        {NULL, NULL, NULL}
//    };
//
//
//    sporth_register_func(&sporth, flist);
///* 0.5 1.5 add 3.0 mul 2.0 mul 4 sub*/
//
//    sporth_stack_push_float(&sporth.stack, 0.5);
//    sporth_stack_push_float(&sporth.stack, 1.5);
//    sporth_exec(&sporth, "add");
//    sporth_stack_push_float(&sporth.stack, 3.0);
//    sporth_exec(&sporth, "mul");
//    sporth_stack_push_float(&sporth.stack, 2.0);
//    sporth_exec(&sporth, "mul");
//    sporth_stack_push_float(&sporth.stack, 4.0);
//    sporth_exec(&sporth, "div");
//    sporth_stack_push_float(&sporth.stack, 1);
//    sporth_exec(&sporth, "sub");
//
//
//    printf("The value is %g!, and the userdata is %g\n",
//        sporth_stack_pop_float(&sporth.stack), GlobalData.val);
//
//    sporth_htable_destroy(&sporth.dict);
//
//    return 0;
//}
