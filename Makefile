#default: hash stacks func
#default: func
default: sporth

UGENS = basic

OBJ += $(addprefix ugens/, $(addsuffix .o, $(UGENS)))

OBJ += func.o plumber.o stack.o parse.o hash.o

%.o: %.c
	gcc -c -Ih $<

ugens/basic.o: ugens/basic.c 
	gcc -Ih -c $< -o $@

sporth: sporth.c $(OBJ)
	gcc sporth.c -Ih -o $@ $(OBJ) -lsoundpipe -lsndfile -lm

clean: 
	rm -rf sporth $(OBJ)

#stacks: stacks.c sporth.h
#	gcc $< -o $@
#hash: hash.c
#	gcc hash.c -o hash
