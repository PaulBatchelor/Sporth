#include "plumber.h"

enum {
    SPACE,
    QUOTE,
    LEX_START,
    LEX_FLOAT,
    LEX_FLOAT_DOT,
    LEX_FLOAT_POSTDOT,
    LEX_POS,
    LEX_NEG,
    LEX_FUNC,
    LEX_ERROR
};

int plumber_init(plumber_data *plumb)
{
    plumb->mode = PLUMBER_CREATE;
    plumb->last = &plumb->root;
    plumb->npipes = 0;
    sporth_stack_init(&plumb->sporth.stack);
    return PLUMBER_OK;
}

int plumber_compute(plumber_data *plumb, int mode)
{
    plumb->mode = mode;
    plumber_pipe *pipe = plumb->root.next, *next;
    uint32_t n;
    float *fval;
    int rc;
    sporth_data *sporth = &plumb->sporth;
    for(n = 0; n < plumb->npipes; n++) {
        next = pipe->next;
        switch(pipe->type) {
            case SPORTH_FLOAT:
                fval = pipe->ud;
                sporth_stack_push_float(&sporth->stack, *fval);
                break;
            default:
                plumb->last = pipe;
                sporth->flist[pipe->type - SPORTH_FOFFSET].func(&sporth->stack, 
                        sporth->flist[pipe->type - SPORTH_FOFFSET].ud);
                break;
        } 
       pipe = next; 
    }
    return PLUMBER_OK;
}

int plumber_show_pipes(plumber_data *plumb)
{
    plumber_pipe *pipe = plumb->root.next, *next;
    uint32_t n;
    float *fval;
    int rc;
    sporth_data *sporth = &plumb->sporth;
    for(n = 0; n < plumb->npipes; n++) {
        next = pipe->next;
        printf("type = %d size = %d", pipe->type, pipe->size);
        if(pipe->type == SPORTH_FLOAT) {
            fval = pipe->ud;
            printf(" val = %g\n", *fval);
        } else {
            printf("\n");
        }
        pipe = next; 
    }
    return PLUMBER_OK;
}

int plumber_clean(plumber_data *plumb)
{
    uint32_t n;
    plumber_compute(plumb, PLUMBER_DESTROY);
    plumber_pipe *pipe, *next;
    pipe = plumb->root.next;
    for(n = 0; n < plumb->npipes; n++) {
        next = pipe->next;
        if(pipe->type == SPORTH_FLOAT || pipe->type == SPORTH_STRING) 
            free(pipe->ud);
        free(pipe);
        pipe = next;
    } 
    sporth_htable_destroy(&plumb->sporth.dict);
    return PLUMBER_OK;
}

int plumber_add_float(plumber_data *plumb, float num)
{
    plumber_pipe *new = malloc(sizeof(plumber_pipe));
  
    if(new == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    new->type = SPORTH_FLOAT;
    new->size = sizeof(SPFLOAT);
    new->ud = malloc(new->size);
    float *val = new->ud;
    *val = num;
    if(new->ud == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    plumb->last->next = new;
    plumb->last = new;
    plumb->npipes++;
    return PLUMBER_OK;
}

int plumber_add_module(plumber_data *plumb, 
        uint32_t id, size_t size, void *ud)
{
    plumber_pipe *new = malloc(sizeof(plumber_pipe));
  
    if(new == NULL) {
        printf("Memory error\n");
        return PLUMBER_NOTOK;
    }

    new->type = id;
    new->size = size;
    new->ud = ud;

    plumb->last->next = new;
    plumb->last = new;
    plumb->npipes++;
    return PLUMBER_OK;
}

int plumber_parse(plumber_data *plumb, const char *filename)
{
    FILE *fp = fopen(filename, "r");

    if(fp == NULL) {
        printf("There was an error reading the file \"%s\"\n", filename);
        return SPORTH_NOTOK;
    }
    char c1 = 0, c2 = 0;
    char str[500];
    uint32_t pos = 0;
    int mode = SPACE;
    int pmode = mode;
    c1 = fgetc(fp);
    while(c2 != EOF){
        c2 = fgetc(fp);
        if(c2 == '\n') c2 = ' ';
        if(c1 == '\n') c1 = ' ';
        if(c1 == '"' && mode != QUOTE) {
            mode = QUOTE;
            c1 = c2;
            c2 = fgetc(fp);
        } 
        while (c1 == ' ' && mode != QUOTE) {
            c1 = c2;
            c2 = fgetc(fp);
        }
        if(mode == SPACE) {
            switch(c2) {
                case ' ':
                    str[pos++] = c1;
                    c1 = fgetc(fp);
                    str[pos] = '\0';
                    plumber_gettype(plumb, str, mode);
                    pos = 0;
                    break;
                default:
                    str[pos++] = c1;
                    c1 = c2;
                    pos %= 500;
                    break;
            }
        } else if (mode == QUOTE) {
            switch(c2) {
                case '"':
                    c2 = fgetc(fp);
                    if(c2 == ' ') {
                        str[pos++] = c1;
                        str[pos] = '\0';
                        plumber_gettype(plumb, str, mode);
                        c1 = fgetc(fp);
                        pos = 0;
                        mode = SPACE;
                    } else {
                        str[pos++] = c1;
                    }
                    break;
                default:
                    str[pos++] = c1;
                    c1 = c2;
                    pos %= 500;
                    break;
            }
        }
    }
    pos--;
    str[pos] = '\0';
    if(pos > 0) plumber_gettype(plumb, str, mode);
    
    fflush(stdout);
    fclose(fp);
    return SPORTH_OK;
}

int plumber_gettype(plumber_data *plumb, char *str, int mode)
{
    int type = LEX_START;
    int sign = LEX_POS;
    char val[101];
    uint32_t module_id;
    strncpy(val, str, 100); 
    if(mode == SPACE) {
        while(*str) {
            switch(type) {
                case LEX_START:
                    switch(str[0]) {
                        case '-':
                            sign = LEX_NEG;
                            type = LEX_FLOAT;
                            break;
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            type = LEX_FLOAT;
                            break;
                        case ' ':
                            type = LEX_START;
                            break;
                        default: 
                            type = LEX_FUNC;
                            break;
                    }
                    break;
                case LEX_FUNC:
                    break;
                case LEX_FLOAT:
                    switch(str[0]) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            type = LEX_FLOAT;
                            break;
                        case '.':
                            type = LEX_FLOAT_DOT;
                            break;
                        default:
                            printf("Syntax error!\n");
                            return -1;
                    }
                    break; 
                  case LEX_FLOAT_DOT:
                  case LEX_FLOAT_POSTDOT:
                    switch(str[0]) {
                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            type = LEX_FLOAT_POSTDOT;
                            break;
                        default:
                            printf("Syntax error!\n");
                            return -1;
                    }
                    break; 

                default:
                    break;
            }
            *str++;
        }
        char signstr[9];
        if(sign == LEX_POS) {
            sprintf(signstr, "positive");
        } else {
            sprintf(signstr, "negative");
        }
        switch(type) {
            case LEX_FLOAT:
            case LEX_FLOAT_DOT:
            case LEX_FLOAT_POSTDOT:
                plumber_add_float(plumb, atof(val));
                break;
            case LEX_FUNC:
                sporth_exec(&plumb->sporth, val);
                break;
            default:
                break;
        }
    } else if (mode == QUOTE) {
       printf(" string!\n"); 
    }
    return 0;
}
