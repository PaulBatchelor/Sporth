#include <stdlib.h>
#include <soundpipe.h>
#include <sporth.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

#include "luasporth.h"

static int luacall(plumber_data *pd, sporth_stack *stack, void **ud)
{
    SPFLOAT trig = 0;
    luacall_d *lcall;
    luasporth_d *ld;
    char *ftname;
    char *sym;
    int i;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            if(sporth_check_args(stack, "fffss") != SPORTH_OK) {
                fprintf(stderr,"Not enough arguments for gain\n");
                stack->error++;
                return PLUMBER_NOTOK;
            }
            lcall = malloc(sizeof(luacall_d));
            *ud = lcall; 
            ftname = sporth_stack_pop_string(stack);
            sym = sporth_stack_pop_string(stack);
            lcall->out = (uint32_t)sporth_stack_pop_float(stack);
            lcall->in = (uint32_t)sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            
            for(i = 0; i < lcall->in; i++) {
                sporth_stack_pop_float(stack);
            }

            for(i = 0; i < lcall->out; i++) {
                sporth_stack_push_float(stack, 0);
            }

            lcall->sym = sym;
            if(plumber_ftmap_search_userdata(pd, 
                ftname, 
                (void **)&ld) != PLUMBER_OK) {
                fprintf(stderr, "Could not find table %s\n", ftname);
            }
            lcall->ld = ld;
            break;

        case PLUMBER_INIT:
            lcall = *ud;
            sporth_stack_pop_string(stack);
            sporth_stack_pop_string(stack);
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            for(i = 0; i < lcall->in; i++) {
                sporth_stack_pop_float(stack);
            }

            for(i = 0; i < lcall->out; i++) {
                sporth_stack_push_float(stack, 0);
            }
            break;

        case PLUMBER_COMPUTE:
            lcall = *ud;
            ld = lcall->ld;
            sporth_stack_pop_float(stack);
            sporth_stack_pop_float(stack);
            trig = sporth_stack_pop_float(stack);
            if(trig != 0) {
                lua_getglobal(ld->L, lcall->sym);
                for(i = 0; i < lcall->in; i++) {
                    lua_pushnumber(ld->L, sporth_stack_pop_float(stack));
                }
                lua_pcall(ld->L, lcall->in, lcall->out, 0);
                for(i = 0; i < lcall->out; i++) {
                    sporth_stack_push_float(stack, 
                        lua_tonumber(ld->L, -1));
                    lua_pop(ld->L, 1);
                }
            } else {
                for(i = 0; i < lcall->in; i++) {
                    sporth_stack_pop_float(stack);
                }
                for(i = 0; i < lcall->out; i++) {
                    sporth_stack_push_float(stack, 0);
                }
            }

            break;

        case PLUMBER_DESTROY:
            lcall = *ud;
            free(lcall);
            break;
        default:
            break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return luacall;
}
