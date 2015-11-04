#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "sporth.h"
#include "ling.h"

#define ENTRY(NAME, FUNC) fl[n].name=NAME;fl[n].func=FUNC;fl[n].ud=NULL; n++;

#define ENTRY_UD(NAME, FUNC, UD) fl[n].name=NAME;fl[n].func=FUNC;fl[n].ud=UD; n++;

void num_to_bin(uint32_t val, char *out, int size) 
{
    uint32_t n;
    for(n = 1; n <= size; n++) {
        out[n - 1] = (val & (1 << (n - 1))) >> (n - 1);
    }
}

static int add(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for add\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v1 + v2);

    return LING_OK;
}

static int sub(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for add\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 - v1);

    return LING_OK;
}

static int mul(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for mul\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 * v1);

    return LING_OK;
}

static int divi(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for mul\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 / v1);

    return LING_OK;
}

static int bw_left(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise left\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 << v1);

    return LING_OK;
}

static int bw_right(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise left\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 >> v1);

    return LING_OK;
}

static int bw_and(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise AND\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 & v1);

    return LING_OK;
}

static int bw_or(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise OR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 | v1);

    return LING_OK;
}

static int bw_xor(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 ^ v1);

    return LING_OK;
}

static int bw_not(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "f") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise NOT\n");
        stack->error++;
    }
    uint32_t v1;

    v1 = ling_stack_pop(stack);

    ling_stack_push(stack, ~v1);

    return LING_OK;
}

static int equal(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 == v1);

    return LING_OK;
}

static int notequal(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 != v1);

    return LING_OK;
}

static int gt(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 > v1);

    return LING_OK;
}

static int lt(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 < v1);

    return LING_OK;
}

static int gtet(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 >= v1);

    return LING_OK;
}

static int ltet(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 <= v1);

    return LING_OK;
}

static int mod(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 % v1);

    return LING_OK;
}

static int and(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 && v1);

    return LING_OK;
}

static int or(ling_stack *stack, void *ud)
{
    if(ling_check_args(stack, "ff") == LING_NOTOK) {
        fprintf(stderr, "not enough args for bitwise XOR\n");
        stack->error++;
    }
    uint32_t v1, v2;

    v1 = ling_stack_pop(stack);
    v2 = ling_stack_pop(stack);

    ling_stack_push(stack, v2 || v1);

    return LING_OK;
}

static int get_t(ling_stack *stack, void *ud)
{
    ling_data *ld = ud;
    ling_stack_push(stack, ld->t);

    return LING_OK;
}

int ling_parse_line(ling_data *ld, char *str) {
    uint32_t pos, len;
    char *out;
    uint32_t size;
    pos = 0;
    len = 0;
    size = strlen(str);

    while(pos < size) {
        out = ling_tokenizer(ld, str, size, &pos);
        len = strlen(out);
        switch(ling_lexer(ld, out, len)) {
            case LING_FLOAT:
                ling_stack_push(&ld->stack, atof(out));
                break;
            case LING_FUNC:
                ling_exec(ld, out);
                break;
            case LING_IGNORE:
                break;
            default:
                fprintf(stderr, "Unknown token %s\n", out);
                break;
        }
        free(out);
    }
    return LING_OK;
}

int ling_register_ugens(ling_data *ld) 
{
    ling_func flist[] = {
        {"+", add, NULL},
        {"-", sub, NULL},
        {"*", mul, NULL},
        {"/", divi, NULL},
        {"<<", bw_left, NULL},
        {">>", bw_right, NULL},
        {"&", bw_and, NULL},
        {"~", bw_not, NULL},
        {"^", bw_xor, NULL},
        {"|", bw_or, NULL},
        {"==", equal, NULL},
        {"!=", notequal, NULL},
        {">", gt, NULL},
        {"<", lt, NULL},
        {"<=", ltet, NULL},
        {">=", gtet, NULL},
        {"%", mod, NULL},
        {"&&", and, NULL},
        {"||", or, NULL},
        {NULL, NULL, NULL}
    };
    ling_register_func(ld, flist);

    return LING_OK;
}

ling_func * ling_create_flist(ling_data *ld) 
{
    uint32_t n = 0;

    ling_func *fl= malloc(sizeof(ling_func) * 21);

    ENTRY("+", add)
    ENTRY("-", sub)
    ENTRY("*", mul)
    ENTRY("/", divi)
    ENTRY("<<", bw_left)
    ENTRY(">>", bw_right)
    ENTRY("&", bw_and)
    ENTRY("~", bw_not)
    ENTRY("^", bw_xor)
    ENTRY("|", bw_or)
    ENTRY("==", equal)
    ENTRY("!=", notequal)
    ENTRY(">", gt)
    ENTRY("<", lt)
    ENTRY("<=", ltet)
    ENTRY(">=", gtet)
    ENTRY("%", mod)
    ENTRY("&&", and)
    ENTRY("||", or)
    ENTRY_UD("t", get_t, ld)
    ENTRY(NULL, NULL)
    return fl;
}

/*
int main() {
    FILE *fp = stdin;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    char out[32];
    uint32_t n;

    ling_data ling;

    n = 0;

    ling_init(&ling);
    ling_func *fl = ling_create_flist();
    ling_register_func(&ling, fl);
    
    while((read = getline(&line, &len, fp)) != -1) {
        parse_line(&ling, line);
    }
    free(line);
    uint32_t val = ling_stack_pop(&ling.stack);

    num_to_bin(val, out, 32);

    printf("%d\n", val);
    for(n = 0; n < 32; n++) 
    printf("%d", out[31 - n]);

    printf("\n");

    ling_destroy(&ling);
    return 0;
}
*/
