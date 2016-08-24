#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "cdb.h"
#include "jsmn.h"

static int parse_json(struct cdb_make *cdbm, char *filename)
{
    FILE *fp = fopen(filename, "r");
    fseek(fp, 0, SEEK_END);
    uint32_t size = ftell(fp);
    char *str = malloc(size + 1);
    fseek(fp, 0, SEEK_SET);
    fread(str, 1, size, fp);
    str[size] = 0;

	int i;
	int r;
	jsmn_parser p;
	jsmntok_t t[128]; 

    jsmn_init(&p);
	r = jsmn_parse(&p, str, size, t, sizeof(t)/sizeof(t[0]));

	if (r < 0) {
		printf("Failed to parse JSON: %d\n", r);
		return 1;
	}

    for(i = 1; i < r; i+=2) {
        if(!strncmp((str + t[i].start), "0x", 2)) {
            char filename[256];
            memset(filename, 0, 256);
            char *tmp;
            strncpy(filename, 
                &str[t[i + 1].start], 
                t[i + 1].end - t[i + 1].start);
            FILE *data = fopen(filename, "rb+");
            if(data == NULL) {
                printf("Couldn't open file %s\n", filename);
            } 
            fseek(data, 0, SEEK_END);
            size_t size = ftell(data);
            tmp = malloc(size * sizeof(char));
            fseek(data, 0, SEEK_SET);
            fread(tmp, sizeof(char), size, data);

            cdb_make_add(cdbm, 
                    &str[t[i].start + 2], 
                    (t[i].end - t[i].start) - 2,
                    tmp, size);
            fclose(data);
            free(tmp);
        } else {
            cdb_make_add(cdbm, 
                    &str[t[i].start], 
                    t[i].end - t[i].start,
                    &str[t[i + 1].start], 
                    t[i + 1].end - t[i + 1].start);
        }
    }

    free(str);
    fclose(fp);
    return 0;
}

int main(int argc, char **argv)
{
    if(argc < 3) {
        fprintf(stderr, "%s out.db file1.json file2.json ... fileN.json\n", 
                argv[0]);
        return 0;
    }

    struct cdb_make cdbm;
    int fd;
    fd = open("tmp.db", O_RDWR | O_CREAT, 0644);
    cdb_make_start(&cdbm, fd);

    argv++;
    argc--;
    char *dbname = *argv;
    argv++;
    argc--;

    int i;
    for(i = 0; i < argc; i++) {
        parse_json(&cdbm, argv[0]); 
        argv++;
    }

    cdb_make_finish(&cdbm);
    rename("tmp.db", dbname);
    close(fd);
    return 0;
}
