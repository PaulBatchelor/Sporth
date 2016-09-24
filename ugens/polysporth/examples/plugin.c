#include <soundpipe.h>
#include <sporth.h>

/* You'll need this header file. It can be found in the Sporth codebase */
#include "../scheme-private.h"

/* macros needed for car/cdr operations */
#define car(p) ((p)->_object._cons._car)
#define cdr(p) ((p)->_object._cons._cdr)

static pointer plugin_function(scheme *sc, pointer args) {
    printf("this is a plugin!\n");
    /* first argument is an integer */
    long i = ivalue(car(args));
    printf("the first argument is %d\n", i);

    /* pop first argument using cdr */
    args = cdr(args);

    /* second argument is a string */
    char *str = string_value(car(args));
    printf("the second argument is %s\n", str);
   
    /* pop the argument */
    args = cdr(args);
    
    /* second argument is a float*/
    SPFLOAT flt  = rvalue(car(args));
    printf("the third argument is %g\n", flt);

    /* return a float */
    return mk_real(sc, 0.2468);
}
void init_plugin(scheme *sc) 
{
    scheme_define(sc, sc->global_env, 
        mk_symbol(sc, "plugin"), 
        mk_foreign_func(sc, plugin_function));
}
