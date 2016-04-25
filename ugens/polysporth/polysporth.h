#include "s7.h"

enum {
PS_ON,
PS_OFF,
PS_NULL,
PS_RELEASE,
PS_TRUE,
PS_FALSE,
PS_OK,
PS_NOTOK,
PS_NOTE,
PS_METANOTE
};

#define NARGS 8

typedef struct sporthlet {
    plumbing pipes;
    int state;
    int id;
    int dur;
    SPFLOAT args[NARGS];
    struct sporthlet *prev, *next;
} sporthlet;

typedef struct dvalue {
    int delta;
    int grp_start;
    int grp_end;
    int dur;
    /* either note or metanote */
    int type;
    /* s7 function if metanote */
    s7_pointer func;
    SPFLOAT *args;
    int nargs;
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

    /* temp list for notelists */
    dvector tmp;
    int noteblock;
    
    sp_ftbl *args;
} polysporth;

int ps_init(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename);
void ps_clean(polysporth *ps);
void ps_compute(polysporth *ps, SPFLOAT tick, SPFLOAT clock);
void dvector_print(dvector *dv);
void dvector_append_value(dvector *dv, dvalue *new);
void dvector_append(dvector *dv, 
    int grp_start, int grp_end, int delta, int dur, 
    SPFLOAT *args, int nargs);
void dvector_append_metanote(dvector *dv, int start, s7_pointer func);
void dvector_init(dvector *dv);
void dvector_free(dvector *dv);
dvector dvector_merge(dvector *dvect1, dvector *dvect2);
//void dvector_pop(dvector *dvect, int *nvoice, dvalue **start);
int dvector_pop(dvector *dvect, dvalue **start);
void dvector_time_to_delta(dvector *dvect);
