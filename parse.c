#include <stdint.h>
#include <stdio.h>
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

int sporth_gettype(sporth_data *sporth, char *str, int mode)
{
    int type = LEX_START;
    int sign = LEX_POS;
    char val[101];
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
                sporth_stack_push_float(&sporth->stack, atof(val));
                break;
            case LEX_FUNC:
                sporth_exec(sporth, val);
        }
    } else if (mode == QUOTE) {
       printf(" string!\n"); 
    }

    return 0;
}

int sporth_parse(sporth_data *sporth , const char *filename) {
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
    //while(!feof(fp)){
    while(c2 != EOF){
        c2 = fgetc(fp);
        if(c2 == '\n') {
            c2 = ' ';
        }
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
                    sporth_gettype(sporth, str, mode);
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
                        sporth_gettype(sporth, str, mode);
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
    if(pos > 0) sporth_gettype(sporth, str, mode);
    
    fflush(stdout);
    fclose(fp);
    return SPORTH_OK;
}
/*
int main() {
    sporth_parse("test.txt");
    return 0;
}
*/
