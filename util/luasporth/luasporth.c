/* gain.c
 * An example of a sporth ugen that can be dynamically loaded using fl, fe, and
 * fc.
 *
 * To compile (on linux):
 * gcc -fPIC -shared gain.c -o gain.so -lsporth
 *
 */

#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>


/* user data. */
typedef struct {
    lua_State *L;
} luasporth_d;

static plumber_data *g_pd = NULL;

static int add_ftable(lua_State *L) 
{
    const char *str = luaL_checkstring(L, 1);
    int size = luaL_checknumber(L, 2);
    int n;
    plumber_data *pd = g_pd;
    sp_ftbl *ft;
    sp_ftbl_create(pd->sp, &ft, size);
    for(n = 1; n <= size; n++) {
        lua_pushnumber(L, n);
        lua_gettable(L, 3);
        ft->tbl[n - 1] = lua_tonumber(L, -1);
    }
    plumber_ftmap_add(pd, str, ft);
    return 0;
}

static int luasporth(plumber_data *pd, sporth_stack *stack, void **ud)
{
    luasporth_d *ls;
    SPFLOAT gain, in;
    char *filename;
    lua_State *L;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            g_pd = pd;
            if(sporth_check_args(stack, "s") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            /* malloc and assign address to user data */
            ls = malloc(sizeof(luasporth_d));
            *ud = ls;
            ls->L = luaL_newstate();
            L = ls->L;
            lua_pushcfunction(L, luaopen_io);
            lua_call(L, 0, 0);
            lua_pushcfunction(L, luaopen_package);
            lua_call(L, 0, 0);
            lua_pushcfunction(L, add_ftable);
            lua_setglobal(L, "add_ftable");
            luaL_openlibs(L);
            luaopen_base(L);
            luaopen_string(L);
            luaopen_math(L);
            luaopen_io(L);
            filename = sporth_stack_pop_string(stack);
            if(luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0)) {
                error(L, "cannot run configuration file: %s", lua_tostring(L, -1));
            }
            free(filename);
            break;

        case PLUMBER_INIT:
            filename = sporth_stack_pop_string(stack);
            free(filename);
            break;

        case PLUMBER_COMPUTE:
            break;

        case PLUMBER_DESTROY:
            ls = *ud;
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
