#include "s7.h"
typedef struct sporthlet {
    plumbing pipes;
    int state;
    /* sp_ftbl *argtbl; */
    int id;
    int dur;
    struct sporthlet *prev, *next;
} sporthlet;

typedef struct dvalue {
    int delta;
    int grp_start;
    int grp_end;
    int dur;
    struct dvalue *next;
} dvalue;

typedef struct dvector {
    int size;
    dvalue root;
    dvalue *last;
} dvector;

typedef struct polysporth {
    s7_scheme *s7;
    plumber_data pd;
    sporthlet *spl;
    int nvoices;
    uint32_t time;
    sp_ftbl *in;
    sp_ftbl *out;
    /* linked list */
    sporthlet root;
    sporthlet *last;

    /* Delta Vector Event list */
    dvector events; 
} polysporth;

int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename);
void ps_clean(polysporth *ps);
void ps_compute(polysporth *ps, SPFLOAT tick);
void dvector_print(dvector *dv);
void dvector_append_value(dvector *dv, dvalue *new);
void dvector_append(dvector *dv, 
    int grp_start, int grp_end, int delta, int dur);
void dvector_init(dvector *dv);
void dvector_free(dvector *dv);
dvector dvector_merge(dvector *dvect1, dvector *dvect2);
void dvector_pop(dvector *dvect, int *nvoice, dvalue **start);
void dvector_reset(dvector *dvect);
