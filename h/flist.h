static sporth_func flist[] = {
    {"add", sporth_add, &plumb_g},
    {"sub", sporth_sub, &plumb_g},
    {"mul", sporth_mul, &plumb_g},
    {"div", sporth_divide, &plumb_g},
    {"sine", sporth_sine, &plumb_g},
    {"c", sporth_constant, &plumb_g},
    {"mix", sporth_mix, &plumb_g},
    {"metro", sporth_metro, &plumb_g},
    {"tenv", sporth_tenv, &plumb_g},
    {"fm", sporth_fm, &plumb_g},
    {"mtof", sporth_mtof, &plumb_g},
    {"dup", sporth_dup, &plumb_g},
    {"swap", sporth_swap, &plumb_g},
    {NULL, NULL, NULL}
};
