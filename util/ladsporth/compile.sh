gcc -nostartfiles ladsporth.c -lm -ldl -lsporth -lsoundpipe -lsndfile -llo -shared -fPIC -o ladsporth.so
#ld -o ladsporth.so ladsporth.o -lsporth -lsoundpipe -lsndfile -lc -lm -fPIC -shared 
