/* luasporth
 *
 * A sporth parser that implements lua scripting using the lua API and
 * libsporth.
 *
 * luasporth functionally is identical to regular sporth, except that
 * the first four lua functions have been tied to lua functions. Lua functions
 * are written in a file called "config.lua" placed in the current working 
 * directory.
 *
 * Each of the lua functions takes a single argument, which is a trigger to 
 * call the lua function. When the lua function is called, it will return
 * a single value, stored indefinitely until the lua function is called agin.
 *
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <soundpipe.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "kona.h"
#include "sporth.h"

typedef struct {
    plumber_data pd;
    lua_State *L;
} UserData;

typedef struct {
    SPFLOAT trig;
    SPFLOAT val;
} ufun0_d;

typedef struct {
    SPFLOAT trig;
    SPFLOAT val;
} ufun1_d;

typedef struct {
    SPFLOAT trig;
    SPFLOAT val;
} ufun2_d;

typedef struct {
    SPFLOAT trig;
    SPFLOAT val;
} ufun3_d;

void process(sp_data *sp, void *ud)
{
    UserData *data = ud;
    plumber_data *pd = &data->pd;
    plumber_compute(pd, PLUMBER_COMPUTE);
    SPFLOAT out;
    sp->out[0] = sporth_stack_pop_float(&pd->sporth.stack);
}

int ufun0(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    UserData *data = pd->ud;
    lua_State *L = data->L;
    ufun0_d *ufd;
    sporth_func_d *fd;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif
            fd = pd->last->ud;
            ufd = malloc(sizeof(ufun0_d));
            fd->ud = ufd;
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;

            ufd->trig = sporth_stack_pop_float(stack);
            ufd->val = 0;
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;
            ufd->trig = sporth_stack_pop_float(stack);
            if(ufd->trig) {
                lua_getglobal(L, "f0");
                //lua_pushnumber(L, 1);
                //lua_pcall(L, 1, 1, 0);
                lua_pcall(L, 0, 1, 0);
                ufd->val = lua_tonumber(L, -1);
            }
            sporth_stack_push_float(stack, ufd->val);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int ufun1(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    UserData *data = pd->ud;
    lua_State *L = data->L;
    ufun1_d *ufd;
    sporth_func_d *fd;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif
            fd = pd->last->ud;
            ufd = malloc(sizeof(ufun0_d));
            fd->ud = ufd;
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;

            ufd->trig = sporth_stack_pop_float(stack);
            ufd->val = 0;
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;
            ufd->trig = sporth_stack_pop_float(stack);
            if(ufd->trig) {
                lua_getglobal(L, "f1");
                lua_pcall(L, 0, 1, 0);
                ufd->val = lua_tonumber(L, -1);
            }
            sporth_stack_push_float(stack, ufd->val);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int ufun2(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    UserData *data = pd->ud;
    lua_State *L = data->L;
    ufun2_d *ufd;
    sporth_func_d *fd;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif
            fd = pd->last->ud;
            ufd = malloc(sizeof(ufun0_d));
            fd->ud = ufd;
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;

            ufd->trig = sporth_stack_pop_float(stack);
            ufd->val = 0;
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;
            ufd->trig = sporth_stack_pop_float(stack);
            if(ufd->trig) {
                lua_getglobal(L, "f2");
                lua_pcall(L, 0, 1, 0);
                ufd->val = lua_tonumber(L, -1);
            }
            sporth_stack_push_float(stack, ufd->val);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int ufun3(sporth_stack *stack, void *ud)
{
    plumber_data *pd = ud;
    UserData *data = pd->ud;
    lua_State *L = data->L;
    ufun3_d *ufd;
    sporth_func_d *fd;

    switch(pd->mode) {
        case PLUMBER_CREATE:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in create mode.\n");
#endif
            fd = pd->last->ud;
            ufd = malloc(sizeof(ufun0_d));
            fd->ud = ufd;
            break;
        case PLUMBER_INIT:

#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in init mode.\n");
#endif

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;

            ufd->trig = sporth_stack_pop_float(stack);
            ufd->val = 0;
            sporth_stack_push_float(stack, 0);

            break;

        case PLUMBER_COMPUTE:

            if(sporth_check_args(stack, "f") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for bpscale\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }

            fd = pd->last->ud;
            ufd = fd->ud;
            ufd->trig = sporth_stack_pop_float(stack);
            if(ufd->trig) {
                lua_getglobal(L, "f3");
                lua_pcall(L, 0, 1, 0);
                ufd->val = lua_tonumber(L, -1);
            }
            sporth_stack_push_float(stack, ufd->val);

            break;

        case PLUMBER_DESTROY:
#ifdef DEBUG_MODE
            fprintf(stderr, "Default user function in destroy mode.\n");
#endif
            fd = pd->last->ud;

            break;

        default:
            fprintf(stderr, "aux (f)unction: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

int main(int argc, char *argv[])
{
    UserData ud;
    ud.L = luaL_newstate();
    lua_State *L = ud.L;
    lua_pushcfunction(L, luaopen_io);
    lua_call(L, 0, 0);
    lua_pushcfunction(L, luaopen_package);
    lua_call(L, 0, 0);
    luaopen_base(L);
    luaopen_string(L);
    luaopen_math(L);

    if(luaL_loadfile(L, "config.lua") || lua_pcall(L, 0, 0, 0))
        error(L, "cannot run configuration file: %s", lua_tostring(L, -1));

    plumber_init(&ud.pd);
    ud.pd.f[0] = ufun0;
    sporth_run(&ud.pd, argc, argv, &ud, process);
    lua_close(L);
    return 0;
}
