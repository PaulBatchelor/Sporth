typedef struct {
    lua_State *L;
    int id;
} luasporth_d;

typedef struct {
    luasporth_d *ld;
    char *sym;
    int in;
    int out;
} luacall_d;
