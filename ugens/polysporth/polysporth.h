#include "s7.h"
typedef struct sporthlet {
    plumbing pipes;
    int state;
    /* sp_ftbl *argtbl; */
    int id;
    int dur;
    struct sporthlet *prev, *next;
} sporthlet;

typedef struct polysporth {
    s7_scheme *s7;
    plumber_data pd;
    sporthlet *spl;
    int nvoices;
    sp_ftbl *in;
    sp_ftbl *out;
    /* linked list */
    sporthlet root;
    sporthlet *last;
} polysporth;

int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename);
void ps_clean(polysporth *ps);
void ps_compute(polysporth *ps, SPFLOAT tick);
