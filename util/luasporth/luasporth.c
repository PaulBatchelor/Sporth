#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luasporth.h"

static int luasporth(plumber_data *pd, sporth_stack *stack, void **ud)
{
    luasporth_d *ls;
    char *filename;
    char *ftname;
    lua_State *L;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "ss") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            /* malloc and assign address to user data */
            ls = malloc(sizeof(luasporth_d));
            *ud = (luasporth_d *)ls;
            ls->L = luaL_newstate();
            L = ls->L;
            ls->id = 123;
            lua_pushcfunction(L, luaopen_io);
            lua_call(L, 0, 0);
            lua_pushcfunction(L, luaopen_package);
            lua_call(L, 0, 0);
            luaL_openlibs(L);
            filename = sporth_stack_pop_string(stack);
            ftname = sporth_stack_pop_string(stack);
            if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
                error(L, "cannot run configuration file: %s", lua_tostring(L, -1));
            }
            plumber_ftmap_delete(pd, 0);
            plumber_ftmap_add_userdata(pd, ftname, ls);
            plumber_ftmap_delete(pd, 1);
            break;

        case PLUMBER_INIT:
            ls = *ud;
            lua_getglobal(ls->L, "init");
            lua_pcall(ls->L, 0, 0, 0);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            break;

        case PLUMBER_COMPUTE:
            break;

        case PLUMBER_DESTROY:
            ls = *ud;
            lua_getglobal(ls->L, "destroy");
            lua_pcall(ls->L, 0, 0, 0);
            lua_close(ls->L);
            free(ls);
            break;
        default:
            fprintf(stderr, "gain: unknown mode!\n");
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return luasporth;
}
