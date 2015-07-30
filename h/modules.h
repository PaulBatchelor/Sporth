int sporth_mix(sporth_stack *stack, void *ud); 
int sporth_constant(sporth_stack *stack, void *ud); 

typedef struct {
    sp_osc *osc;
    sp_ftbl *ft;
} sporth_sine_d;

int sporth_sine(sporth_stack *stack, void *ud); 
int sporth_add(sporth_stack *stack, void *ud);
int sporth_mul(sporth_stack *stack, void *ud);
int sporth_sub(sporth_stack *stack, void *ud);
int sporth_divide(sporth_stack *stack, void *ud);
int sporth_metro(sporth_stack *stack, void *ud);
int sporth_tenv(sporth_stack *stack, void *ud);
