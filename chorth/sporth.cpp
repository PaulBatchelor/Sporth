#include "chuck_dl.h"
#include "chuck_def.h"
#include <stdio.h>
#include <limits.h>

extern "C"{
#include "soundpipe.h"
#include "../tmp.h"
#ifdef BUILD_KONA
#include "../kona/kona.h"
#endif
}


CK_DLL_CTOR(sporth_ctor);
CK_DLL_DTOR(sporth_dtor);

CK_DLL_MFUN(sporth_setp);
CK_DLL_MFUN(sporth_getp);

CK_DLL_MFUN(sporth_get_table);
CK_DLL_MFUN(sporth_set_table);

CK_DLL_MFUN(sporth_parse_string);

CK_DLL_TICK(sporth_get_tick);


t_CKINT sporth_data_offset = 0;

struct sporthData {
    float var;
    int parsed;
    sp_data *sp;
    plumber_data pd;
    SPFLOAT in;
};

extern "C" {
static int sporth_chuck_in(sporth_stack *stack, void *ud);
}

CK_DLL_QUERY(Sporth)
{
    QUERY->setname(QUERY, "Sporth");
    
    QUERY->begin_class(QUERY, "Sporth", "UGen");
    
    QUERY->add_ctor(QUERY, sporth_ctor);
    QUERY->add_dtor(QUERY, sporth_dtor);
    
    QUERY->add_ugen_func(QUERY, sporth_get_tick, NULL, 1, 1);
    
    QUERY->add_mfun(QUERY, sporth_setp, "float", "pset");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->add_arg(QUERY, "float", "val");
    
    QUERY->add_mfun(QUERY, sporth_getp, "float", "pget");
    QUERY->add_arg(QUERY, "int", "index");
    
    QUERY->add_mfun(QUERY, sporth_set_table, "float", "tset");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->add_arg(QUERY, "float", "val");
    QUERY->add_arg(QUERY, "string", "table");
    
    QUERY->add_mfun(QUERY, sporth_get_table, "float", "tget");
    QUERY->add_arg(QUERY, "int", "index");
    QUERY->add_arg(QUERY, "string", "table");
    
    QUERY->add_mfun(QUERY, sporth_parse_string, "string", "parse");
    QUERY->add_arg(QUERY, "string", "arg");
    
    sporth_data_offset = QUERY->add_mvar(QUERY, "int", "@sporth_data", false);
    
    QUERY->end_class(QUERY);

    return TRUE;
}


CK_DLL_CTOR(sporth_ctor)
{
    OBJ_MEMBER_INT(SELF, sporth_data_offset) = 0;
   
    printf("Creating Sporth\n");
    sporthData * data = new sporthData;
    data->parsed = 0;
    data->in = 0;
    sp_create(&data->sp);
    sp_srand(data->sp, rand());
    data->sp->sr = API->vm->get_srate();
    plumber_register(&data->pd);
    data->pd.sporth.flist[SPORTH_IN - SPORTH_FOFFSET].func = sporth_chuck_in;
    plumber_init(&data->pd);
    data->pd.sp = data->sp;
    data->pd.ud = data;

    OBJ_MEMBER_INT(SELF, sporth_data_offset) = (t_CKINT) data;
}

CK_DLL_DTOR(sporth_dtor)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    if(data)
    {
        printf("Destroying Sporth\n");
        plumber_clean(&data->pd);
        sp_destroy(&data->sp);

        delete data;
        OBJ_MEMBER_INT(SELF, sporth_data_offset) = 0;
        data = NULL;
    }
}

CK_DLL_TICK(sporth_get_tick)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    data->in = in;
    plumber_compute(&data->pd, PLUMBER_COMPUTE);
    *out = sporth_stack_pop_float(&data->pd.sporth.stack);
    return TRUE;
}

CK_DLL_MFUN(sporth_setp)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    int i = GET_NEXT_INT(ARGS);
    float val = GET_NEXT_FLOAT(ARGS);
    data->pd.p[i] = val;
    RETURN->v_float = val;
}

CK_DLL_MFUN(sporth_set_table)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);


    int i = GET_NEXT_INT(ARGS);
    float val = GET_NEXT_FLOAT(ARGS);
    
    Chuck_String * ckstring = GET_CK_STRING(ARGS);
    const char * cstr = ckstring->str.c_str();
    char *ftname= (char *)malloc(strlen(cstr) + 1);
    ftname = strdup(cstr);

    int err = 0;

    sp_ftbl *ft;

    if(plumber_ftmap_search(&data->pd, ftname, &ft) == PLUMBER_NOTOK) {
        fprintf(stderr, "Could not find ftable %s", ftname);
        err++;
    }

    if(i > ft->size) {
        fprintf(stderr, "Stack overflow! %d exceeds table size %d\n", i, ft->size);
        err++;
    }

    if(!err) {
        ft->tbl[i] = val;
    }

    free(ftname);
    RETURN->v_float = val;
}

CK_DLL_MFUN(sporth_get_table)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);


    int err = 0;
    int i = GET_NEXT_INT(ARGS);

    sp_ftbl *ft;
    
    Chuck_String * ckstring = GET_CK_STRING(ARGS);
    const char * cstr = ckstring->str.c_str();
    char *ftname= (char *)malloc(strlen(cstr) + 1);
    ftname = strdup(cstr);

    if(plumber_ftmap_search(&data->pd, ftname, &ft) == PLUMBER_NOTOK) {
        fprintf(stderr, "Could not find ftable %s", ftname);
        err++;
    }

    if(i > ft->size) {
        fprintf(stderr, "Stack overflow! %d exceeds table size %d\n", i, ft->size);
        err++; 
    }

    free(ftname);

    if(!err) {
        RETURN->v_float = ft->tbl[i];
    } else {
        RETURN->v_float = 0;
    }
}

CK_DLL_MFUN(sporth_getp)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    int i = GET_NEXT_INT(ARGS);
    RETURN->v_float = data->pd.p[i];
}

CK_DLL_MFUN(sporth_parse_string)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    Chuck_String * ckstring = GET_CK_STRING(ARGS);
    const char * cstr = ckstring->str.c_str();
    char *str = (char *)malloc(strlen(cstr) + 1);
    str = strdup(cstr);
    if(!data->parsed) {
        data->parsed = 1;
        plumber_parse_string(&data->pd, str);
        plumber_compute(&data->pd, PLUMBER_INIT);
    } else {
        plumber_recompile_string(&data->pd, str);
    }
    free(str);
    RETURN->v_float = data->var;
}

extern "C" {
static int sporth_chuck_in(sporth_stack *stack, void *ud)
{
    plumber_data *pd = (plumber_data *) ud;

    sporthData * data = (sporthData *) pd->ud;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: creating\n");
#endif
            plumber_add_ugen(pd, SPORTH_IN, NULL);

            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: initialising.\n");
#endif

            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            sporth_stack_push_float(stack, data->in);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "CHUCK IN: destroying.\n");
#endif

            break;

        default:
            fprintf(stderr, "Unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}
}
