/*

Simple debugger tool for Sporth.

Usage: sporth -b raw -c 1 -d 1 in.sp | ./val

*/
#include <stdio.h>

int main()
{
    float x;
    while(fread(&x, sizeof(float), 1, stdin) != 0) {
        printf("%g ", x);
    }
    printf("\n");
    return 0;
}
