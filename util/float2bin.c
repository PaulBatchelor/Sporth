/* Float2bin
 *
 * Convert text table of floats into binary values, and write to stdout.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    FILE *fp = stdin;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    char *str1 = NULL, *str2, *token = NULL;
    char *saveptr = NULL;
    char space[1];
    space[0] = ' ';
    int j;
    float val = 0;
    while((read = getline(&line, &len, fp)) != -1) {
        for(j = 1, str1 = line; ;j++, str1 = NULL) {
            token = strtok_r(str1, space, &saveptr);
            if(token == NULL) break;
            val = atof(token);
            fwrite(&val, sizeof(float), 1, stdout);
        }
    }

    free(line);
    return 0;
}
