static sporth_func flist[] = {
    {"add", sporth_add, NULL},
    {"sub", sporth_sub, NULL},
    {"mul", sporth_mul, NULL},
    {"div", sporth_divide, NULL},
    {"sine", sporth_sine, &plumb_g},
    {"c", sporth_constant, &plumb_g},
    {"mix", sporth_mix, &plumb_g},
    {NULL, NULL, NULL}
};
