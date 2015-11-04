#include <stdlib.h>
#include <stdint.h>

enum {
LING_FLOAT,
LING_IGNORE,
LING_FUNC,
LING_NOTOK,
LING_OK
};

#define SPORTH_FOFFSET 2
#define SPORTH_MAXCHAR 200

//typedef struct {
//    uint32_t fval;
//    char sval[SPORTH_MAXCHAR];
//    int type;
//} ling_stack_val;

typedef struct {
    int pos;
    uint32_t error;
    uint32_t stack[32];
} ling_stack;

typedef struct {
    const char *name;
    int (*func)(ling_stack *, void *);
    void *ud;
} ling_func;

typedef struct ling_data{
    sporth_htable dict;
    uint32_t nfunc;
    ling_func *flist;
    ling_stack stack;
    uint32_t t;
} ling_data;

int ling_stack_init(ling_stack *stack);
int ling_stack_push(ling_stack *stack, uint32_t val);
uint32_t ling_stack_pop(ling_stack *stack);

int ling_check_args(ling_stack *stack, const char *args);
int ling_register_func(ling_data *ld, ling_func *flist);
int ling_exec(ling_data *ld, const char *keyword);
int ling_init(ling_data *ld);
int ling_destroy(ling_data *ld);

int ling_parse(ling_data *ld, const char *filename);
char * ling_tokenizer(ling_data *ld, char *str,
        uint32_t size, uint32_t *pos);
int ling_lexer(ling_data *ld, char *str, int32_t size);

ling_func * ling_create_flist(ling_data *ld);
