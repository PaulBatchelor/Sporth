#include "chuck_dl.h"
#include "chuck_def.h"
#include <stdio.h>
#include <limits.h>

extern "C"{
#include "../soundpipe.h"
#include "../tmp.h"
}


CK_DLL_CTOR(sporth_ctor);
CK_DLL_DTOR(sporth_dtor);

CK_DLL_MFUN(sporth_setVar);
CK_DLL_MFUN(sporth_getVar);

CK_DLL_MFUN(sporth_parseString);

CK_DLL_TICK(sporth_tick);

t_CKINT sporth_data_offset = 0;

struct sporthData {
    float var;
    int parsed;
    sp_data *sp;
    plumber_data pd;
};
CK_DLL_QUERY(Sporth)
{
    QUERY->setname(QUERY, "Sporth");
    
    QUERY->begin_class(QUERY, "Sporth", "UGen");
    
    QUERY->add_ctor(QUERY, sporth_ctor);
    QUERY->add_dtor(QUERY, sporth_dtor);
    
    QUERY->add_ugen_func(QUERY, sporth_tick, NULL, 1, 1);
    
    QUERY->add_mfun(QUERY, sporth_setVar, "float", "var");
    QUERY->add_arg(QUERY, "float", "arg");
    
    QUERY->add_mfun(QUERY, sporth_getVar, "float", "var");
    
    QUERY->add_mfun(QUERY, sporth_parseString, "string", "parse");
    QUERY->add_arg(QUERY, "string", "arg");
    
    sporth_data_offset = QUERY->add_mvar(QUERY, "int", "@sporth_data", false);
    
    QUERY->end_class(QUERY);

    return TRUE;
}


CK_DLL_CTOR(sporth_ctor)
{
    OBJ_MEMBER_INT(SELF, sporth_data_offset) = 0;
   
    sporthData * data = new sporthData;
    data->parsed = 0;
    sp_create(&data->sp);
    data->sp->sr = API->vm->get_srate();
    plumber_register(&data->pd);
    plumber_init(&data->pd);
    data->pd.sp = data->sp;


    OBJ_MEMBER_INT(SELF, sporth_data_offset) = (t_CKINT) data;
}

CK_DLL_DTOR(sporth_dtor)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    if(data)
    {
        plumber_clean(&data->pd);
        sp_destroy(&data->sp);

        delete data;
        OBJ_MEMBER_INT(SELF, sporth_data_offset) = 0;
        data = NULL;
    }
}

CK_DLL_TICK(sporth_tick)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    plumber_compute(&data->pd, PLUMBER_COMPUTE);
    *out = sporth_stack_pop_float(&data->pd.sporth.stack);
    return TRUE;
}

CK_DLL_MFUN(sporth_setVar)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    data->var = GET_NEXT_FLOAT(ARGS);
    RETURN->v_float = data->var;
}

CK_DLL_MFUN(sporth_getVar)
{
    sporthData * data = (sporthData *) OBJ_MEMBER_INT(SELF, sporth_data_offset);
    RETURN->v_float = data->var;
}

CK_DLL_MFUN(sporth_parseString)
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
    }
    free(str);
    RETURN->v_float = data->var;
}


