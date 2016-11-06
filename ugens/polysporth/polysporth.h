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
PS_METANOTE,
PS_NOTEOFF,
PS_PLEASE_SHUTUP
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
    pointer func;
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
    scheme sc;
    plumber_data pd;
    sporthlet *spl;
    int nvoices;
    uint32_t time;
    uint32_t reltime;
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

    /* pointers for callbacks */
    pointer cb;
    pointer shutdown;
    pointer init;

    /* current voice */
    int id;
} polysporth;

int ps_create(plumber_data *pd, sporth_stack *stack, polysporth *ps, int ninstances, char *in_tbl, 
    char *out_tbl, char *filename);
void ps_init(polysporth *ps);
void ps_clean(polysporth *ps);
void ps_compute(polysporth *ps, SPFLOAT tick, SPFLOAT clock);
void dvector_print(dvector *dv);
void dvector_append_value(dvector *dv, dvalue *new);
void dvector_append(dvector *dv, 
    int grp_start, int grp_end, int delta, int dur, 
    SPFLOAT *args, int nargs);
void dvector_append_metanote(dvector *dv, int start, pointer func);
void dvector_init(dvector *dv);
void dvector_free(dvector *dv);
dvector dvector_merge(dvector *dvect1, dvector *dvect2);
int dvector_pop(dvector *dvect, dvalue **start);
void dvector_time_to_delta(dvector *dvect);
void ps_turnon_sporthlet(polysporth *ps, int id, int dur);
void ps_turnoff_sporthlet(polysporth *ps, int id);
void ps_scm_load(polysporth *ps, char *filename);
void ps_set_arg(polysporth *ps, int id, int pos, SPFLOAT val);
void ps_sporthlet_noteoff(polysporth *ps, int id);
