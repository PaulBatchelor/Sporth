#include "sporth.h"

int sporth_stack_push_float(sporth_stack *stack, float val)
{
    if(stack->error > 0) return SPORTH_NOTOK;

    if(stack->pos <= 32) {
        //printf("Pushing value %g.\n", val);
        stack->pos++;
        stack->stack[stack->pos - 1].fval = val;
        return SPORTH_OK;
    } else {
        printf("Stack limit of %d reached, cannot push float value.\n", stack->pos);
        stack->error++;
        return SPORTH_NOTOK; 
    }
}

int sporth_stack_push_string(sporth_stack *stack, const char *str)
{
    if(stack->error > 0) return SPORTH_NOTOK;

    sporth_stack_val *pstack;
    if(stack->pos <= 32) {
        stack->pos++;
        pstack = &stack->stack[stack->pos - 1];
        strcpy(pstack->sval, str);
        pstack->fval = strlen(str);
        pstack->type = SPORTH_STRING;
        return SPORTH_OK;
    } else {
        printf("Stack limit of %d reached, cannot push float value.\n", stack->pos);
        stack->error++;
        return SPORTH_NOTOK;
    }
    return SPORTH_OK;
}

float sporth_stack_pop_float(sporth_stack *stack)
{
    if(stack->error > 0) return 0;

    sporth_stack_val *pstack;

    if(stack->pos == 0) {
       printf("Stack is empty.\n");
       stack->error++;
       return SPORTH_NOTOK;
    }
    pstack = &stack->stack[stack->pos - 1];

    if(pstack->type != SPORTH_FLOAT) {
        printf("Value is not a float.\n");
        stack->error++;
        return SPORTH_NOTOK;
    }

    stack->pos--;
    return pstack->fval;
}

char * sporth_stack_pop_string(sporth_stack *stack)
{
    if(stack->error > 0) return NULL;

    char *str;
    sporth_stack_val *pstack;

    if(stack->pos == 0) {
       printf("Stack is empty.\n");
       stack->error++;
       return NULL;
    }
    pstack = &stack->stack[stack->pos - 1];

    if(pstack->type != SPORTH_STRING) {
        printf("Value is not a string.\n");
        stack->error++;
        return NULL;
    }

    str = malloc(sizeof(char) * (pstack->fval + 1));
    strcpy(str, pstack->sval);
    stack->pos--;
    return str;
}

int sporth_stack_init(sporth_stack *stack)
{
    stack->pos = 0;
    stack->error = 0;
    return SPORTH_OK;
}

/*
int main()
{
    sporth_stack stack;
    sporth_stack_init(&stack);
    sporth_stack_pop_float(&stack);
    sporth_stack_push_float(&stack, 123);
    sporth_stack_push_float(&stack, 456);
    sporth_stack_push_string(&stack, "HI GUYS");
    sporth_stack_push_float(&stack, 789);

    float val = sporth_stack_pop_float(&stack);
    char *str = sporth_stack_pop_string(&stack);

    printf("Float val is %g and string is %s\n", val, str);

    if(str != NULL) free(str);

    return 0;
}
*/
