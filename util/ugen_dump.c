#include <stdio.h>
#include "plumber.h"

int main()
{
#define SPORTH_UGEN(key, func, macro, ninputs, noutputs) \
    fprintf(stdout, "%d %s\n", macro - SPORTH_FOFFSET, key);
#include "ugens.h"
#undef SPORTH_UGEN
    return 0;
}
