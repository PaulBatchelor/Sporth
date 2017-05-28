/* sporth_tex.c
 *
 * This program parses a sporth file and generates plain tex code with 
 * syntax highlighting. 
 *
 * Example usage:
 *
 *      sporth_tex in.sp > code.tex
 *      tex "\nopagenumbers \input code \bye"
 *      dvipng code
 *     
 * This will typically generate one page of output to "code1.png"
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "plumber.h"

static void escape_me(char *out)
{
    int i;
    for(i = 0; i < strlen(out); i++) {
        switch(out[i]) {
            case '|':
            case '+':
            case '-':
            case '*':
                printf("$%c$", out[i]);
                break;
            case '_':
            case '#':
                printf("\\%c", out[i]);
            break;
            default:
                printf("%c", out[i]);
        }
    }
}

static int lexer(char *out, uint32_t len)
{
    switch(sporth_lexer(out, len)) {
        case SPORTH_FLOAT:
            printf("{\\sporthfloat ");
            escape_me(out);
            printf("}\n");
            break;
        case SPORTH_STRING:
            printf("{\\sporthstring ");
            escape_me(out);
            printf("}\n");
            break;
        case SPORTH_WORD:
            printf("{\\sporthword ");
            escape_me(out);
            printf("}\n");
            break;
        case SPORTH_FUNC:
            printf("{\\sporthugen ");
            escape_me(out);
            printf("}\n");
            break;
        case SPORTH_IGNORE:
            printf("{\\sporthcomment ");
            escape_me(out);
            printf("}\n");
            break;
        default:
            return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    FILE *fp;
    char *line;
    size_t length;
    ssize_t read;
    char *out;
    uint32_t nugens;
    
    uint32_t pos = 0, len = 0;

    if(argc == 1) {
        fprintf(stderr, "Usage: %s in.sp\n", argv[0]);
        exit(0);
    }

    fp = fopen(argv[1], "r");

    line = NULL;
    len = 0;
    if(fp == NULL) {
        fprintf(stderr, "Couldn't find file.");
        exit(1);
    }

    printf("\\begingroup\n");
    printf("\\def \\sporthfloat {} \n");
    printf("\\def \\sporthugen {\\bf} \n");
    printf("\\def \\sporthword {\\sl} \n");
    printf("\\def \\sporthstring {\\it} \n");
    printf("\\def \\sporthcomment{\\tt} \n");
    printf("\n");
    while((read = sporth_getline(&line, &length, fp)) != -1) {
        pos = 0;
        len = 0;
        nugens = 0;
        printf("\\noindent\n");
        while(pos < read - 1) {
            out = sporth_tokenizer(line, (unsigned int)read - 1, &pos);
            len = (unsigned int)strlen(out);
            nugens += lexer(out, len);
            free(out);
        }
        if(nugens == 0) {
            printf("\\medskip\n");
        }
        printf("\n");
    }
    printf("\\endgroup");

    fclose(fp);
    free(line);
    return 0;
}
