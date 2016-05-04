/* Example code for building a plugin loadable by polysporth 
 * to compile: 
 *
 * gcc -shared -IS7_HEADER_LOCATION -fPIC plugin.c -o plugin.so
 * 
 * where S7_HEADER_LOCATION is where the s7 header files are
 *
 * to load (via polysporth):
 *
 * (cload "./plugin.so" "init_ex")
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s7.h"

static s7_pointer add_1(s7_scheme *sc, s7_pointer args)
{
    return (s7_make_integer(sc, s7_integer(s7_car(args)) + 1));
}

void init_ex(s7_scheme *sc)
{
    s7_define_function(sc, "add-1", add_1, 1, 0, false, NULL);
}
