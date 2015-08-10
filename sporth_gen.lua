sptbl = {}

dofile("sp_dict.lua")

UGen = { name = module }

function UGen.debug(self, str, spaces)
    io.write("\n#ifdef DEBUG_MODE\n")
    io.write(string.format("%sfprintf(stderr, \"%s\")\n", spaces, str))
    io.write("#endif\n\n")
end

function UGen.header(self, sp)
    io.write("#include \"plumber.h\"\n\n")
    io.write(string.format("int sporth_%s(sporth_stack *stack, void *ud)\n", self.name))
    io.write("{\n")
    io.write("    plumber_data *pd = ud;\n");
end

function UGen.variables(self, sp)
    for i = 1, sp.ninputs do
        io.write(string.format("    SPFLOAT %s;\n", sp.inputs[i].name))
    end
    for i = 1, sp.noutputs do
        io.write(string.format("    SPFLOAT %s;\n", sp.outputs[i].name))
    end

    if(sp.params.mandatory ~= nil) then
        for _,v in pairs(sp.params.mandatory) do
            io.write(string.format("    %s %s;\n", v.type, v.name))
        end
    end
    if(sp.params.optional ~= nil) then
        for _,v in pairs(sp.params.optional) do
            io.write(string.format("    %s %s;\n", v.type, v.name))
        end
    end
    io.write(string.format("    sp_%s *%s;\n", self.name, self.name));
end

function UGen.switch(self, sp) io.write("\n    switch(pd->mode) {\n")
end

function UGen.create(self, sp, macro)
    spaces = "        "
    indent = spaces .. "    "
    io.write(string.format("%scase PLUMBER_CREATE:\n", spaces))
    self:debug(string.format("%s: Creating\\n", self.name), indent)
    io.write(string.format("%ssp_%s_create(&%s);\n", indent, self.name, self.name, self.name))
    io.write(string.format("%splumber_add_module(pd, %s, sizeof(sp_%s), %s);\n",
        indent, macro, self.name, self.name, self.name))
    io.write(string.format("%sbreak;\n", indent));
end

function UGen.pop(self, sp)
    spaces = "        "
    indent = spaces .. "    "
    if(sp.params.mandatory ~= nil) then
        for _,v in pairs(sp.params.mandatory) do
            io.write(string.format("%s%s = sporth_stack_pop_float(stack);\n", spaces, v.name))
        end
    end

    if(sp.params.optional ~= nil) then
        tbl = {}
        size = 0
        for _,v in pairs(sp.params.optional) do
            table.insert(tbl, v)
            --io.write(string.format("%s%s = sporth_stack_pop_float(stack);\n", indent, v.name))
            size = size + 1
        end
        --for _,v in pairs(tbl) do
        for i = size, 1, -1 do
            io.write(string.format("%s%s = sporth_stack_pop_float(stack);\n", indent, tbl[i].name))
        end

    end

    for i = 1, sp.ninputs do
        io.write(string.format("%s%s = sporth_stack_pop_float(stack);\n", indent, sp.inputs[i].name))
    end
end

function UGen.init(self, sp, args)
    spaces = "        "
    indent = spaces .. "    "
    indent2 = indent .. "    "

    io.write(string.format("%scase PLUMBER_INIT:\n", spaces));
    self:debug(string.format("%s: Initialising\\n", self.name), indent)
    io.write(string.format("%sif(sporth_check_args(stack, \"%s\") != SPORTH_OK) {\n",
        indent, args))
    io.write(string.format("%sfprintf(stderr,\"Not enough arguments for %s\\n\");\n",
        indent2, self.name))
    io.write(string.format("%sstack->error++;\n", indent2))
    io.write(string.format("%sreturn PLUMBER_NOTOK;\n", indent2))
    io.write(string.format("%s}\n", indent))
    self:pop(sp)
    io.write(string.format("%s%s = pd->last->ud;\n", indent, self.name))
    io.write(string.format("%ssp_%s_init(pd->sp, %s", indent, self.name, self.name))

    if(sp.params.mandatory ~= nil) then
        for _,v in pairs(sp.params.mandatory) do
            io.write(string.format(",%s ", v.name))
        end
    end
    io.write(");\n")
    for i = 1, sp.noutputs do
    io.write(string.format("%ssporth_stack_push_float(stack, 0);\n", indent))
    end
    io.write(string.format("%sbreak;\n", indent))
end

function UGen.compute(self, sp, args)
    spaces = "        "
    indent = spaces .. "    "
    indent2 = indent .. "    "

    io.write(string.format("%scase PLUMBER_COMPUTE:\n", spaces));
    io.write(string.format("%sif(sporth_check_args(stack, \"%s\") != SPORTH_OK) {\n",
        indent, args))
    io.write(string.format("%sfprintf(stderr,\"Not enough arguments for %s\\n\");\n",
        indent2, self.name))
    io.write(string.format("%sstack->error++;\n", indent2))
    io.write(string.format("%sreturn PLUMBER_NOTOK;\n", indent2))
    io.write(string.format("%s}\n", indent))
    self:pop(sp)
    io.write(string.format("%s%s = pd->last->ud;\n", indent, self.name))

    if(sp.params.optional ~= nil) then
        for _,v in pairs(sp.params.optional) do
            io.write(string.format("%s%s->%s = %s;\n", indent, self.name, v.name, v.name))
        end
    end

    io.write(string.format("%ssp_%s_compute(pd->sp, %s", indent, self.name, self.name))

    if(sp.ninputs == 0) then
        io.write(", NULL")
    else
        for i = 1, sp.ninputs do
            io.write(string.format(", &%s", sp.inputs[i].name))
        end
    end

    for i = 1, sp.noutputs do
        io.write(string.format(", &%s", sp.outputs[i].name))
    end

    io.write(");\n")
    for i = 1, sp.noutputs do
    io.write(string.format("%ssporth_stack_push_float(stack, %s);\n",
        indent, sp.outputs[i].name))
    end
    io.write(string.format("%sbreak;\n", indent))
end

function UGen.destroy(self, sp)
    spaces = "        "
    indent = spaces .. "    "

    io.write(string.format("%scase PLUMBER_DESTROY:\n", spaces));
    io.write(string.format("%s%s = pd->last->ud;\n", indent, self.name))
    io.write(string.format("%ssp_%s_destroy(&%s);\n", indent, self.name, self.name))
    io.write(string.format("%sbreak;\n", indent))
end

function UGen.default(self,sp)
    spaces = "        "
    indent = spaces .. "    "
    io.write(string.format("%sdefault:\n", spaces));
    io.write(string.format("%sfprintf(stderr, \"%s: Uknown mode!\\n\");\n",
        indent, self.name))
    io.write(string.format("%sbreak;\n", indent))
end

function UGen.footer(self, sp)
    io.write("    }\n")
    io.write("    return PLUMBER_OK;\n");
    io.write("}\n")
end

name = arg[1]
macro = arg[2]
args = arg[3]

sp = sptbl[name]
UGen.name = name

UGen:header(sp)
UGen:variables(sp)
UGen:switch(sp)
UGen:create(sp, macro)
UGen:init(sp, args)
UGen:compute(sp, args)
UGen:destroy(sp)
UGen:default(sp)
UGen:footer(sp)
